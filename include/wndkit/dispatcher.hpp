// Copyright (c) 2025 Bevan Collins
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include <windows.h>
#include <optional>
#include <system_error>
#include <unordered_map>
#include <cassert>
#include "message_handler.hpp"

namespace wndkit {

class dispatcher {
public:
  /*
      Create a window and attach a message handler
   */
  static HWND create_window(message_handler& handler, DWORD ex_style, const wchar_t* class_name, const wchar_t* window_name, DWORD style, int x, int y, int width, int height, HWND parent, HMENU menu, HINSTANCE instance, LPVOID params) {
    create_window_params param_shim{handler, params};
    return CreateWindowExW(ex_style, class_name, window_name, style, x, y, width, height, parent, menu, instance, &param_shim);
  }

  /*
      Create a dialog and attach a message handler
   */
  static INT_PTR dialog_box_indirect_param(message_handler& handler, HINSTANCE instance, LPCDLGTEMPLATEW dialog_template, HWND parent, LPARAM init_param) {
    dialog_box_indirect_params param_shim{handler, init_param};
    return DialogBoxIndirectParamW(instance, dialog_template, parent, &dialog_proc, reinterpret_cast<LPARAM>(&init_param));
  }

  /*
      Create and subclass a window, then attach a message handler
   */
  static HWND create_subclass_window(message_handler& handler, DWORD ex_style, const wchar_t* class_name, const wchar_t* window_name, DWORD style, int x, int y, int width, int height, HWND parent, HMENU menu, HINSTANCE instance, LPVOID params, UINT_PTR id_subclass = {}, DWORD_PTR ref_data = {}) {
    assert(class_name);

    auto hwnd = CreateWindowExW(ex_style, class_name, window_name, style, x, y, width, height, parent, menu, instance, params);
    if (hwnd) {
      if (!SetWindowSubclass(hwnd, &sub_class_proc, id_subclass, ref_data))
        throw std::system_error(static_cast<int>(GetLastError()), std::system_category());

      attach_window(hwnd, handler);
    }

    return hwnd;
  }

  /*
     The standard Windows event loop
  */
  static int run() noexcept(false) {
    for(;;) {
      MSG msg;
      auto ret = GetMessageW(&msg, 0, 0, 0);
      if (ret == -1)
        throw std::system_error(static_cast<int>(GetLastError()), std::system_category());
      else if (ret == 0) {
        quit_params params{msg.wParam, msg.lParam};
        return params.exit_code();
      }

      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }
  }

  static LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    auto result = call_handler(hwnd, msg, wparam, lparam);
    return result.value_or(DefWindowProcW(hwnd, msg, wparam, lparam));
  }

private:
  static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    auto result = call_handler(hwnd, msg, wparam, lparam);
    return result.value_or(FALSE);
  }

  static LRESULT CALLBACK sub_class_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, [[maybe_unused]] UINT_PTR id_subclass, [[maybe_unused]] DWORD_PTR ref_data) {
    auto result = call_handler(hwnd, msg, wparam, lparam);
    return result.value_or(DefSubclassProc(hwnd, msg, wparam, lparam));
  }

  static auto& handlers() {
    thread_local static std::unordered_map<HWND, message_handler&> handlers_;
    return handlers_;
  }

  static void attach_window(HWND hwnd, message_handler& handler) {
    assert(hwnd);

    auto inserted = handlers().insert({hwnd, handler});
    assert(inserted.second);
  }

  struct create_window_params {
    message_handler& handler;
    LPVOID original_create_params;
  };

  struct dialog_box_indirect_params {
    message_handler& handler;
    LPARAM original_init_param;
  };

  static std::optional<LRESULT> call_handler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_NCCREATE) {
      nccreate_params params{wparam, lparam};
      auto create_params = reinterpret_cast<create_window_params*>(params.createstruct()->lpCreateParams);
      params.createstruct()->lpCreateParams = create_params->original_create_params;

      auto inserted = handlers().insert({hwnd, create_params->handler});
      assert(inserted.second);

      auto ret = create_params->handler.call_handler(hwnd, msg, wparam, lparam);

      // if the WM_NCCREATE handler returns FALSE then remove the handler
      if (ret.has_value() && ret.value() == 0)
        handlers().erase(inserted.first);

      return ret;
    } else if (msg == WM_INITDIALOG) {
      auto init_params = reinterpret_cast<dialog_box_indirect_params*>(lparam);

      auto inserted = handlers().insert({hwnd, init_params->handler});
      assert(inserted.second);

      return init_params->handler.call_handler(hwnd, msg, wparam, init_params->original_init_param);
    } else {
      auto match = handlers().find(hwnd);
      if (match == handlers().end()) {
        return std::nullopt;
      } else {
        auto ret = match->second.call_handler(hwnd, msg, wparam, lparam);

        if (msg == WM_NCDESTROY)
          handlers().erase(match);

        return ret;
      }
    }
  }
};

}
