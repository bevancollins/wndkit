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
  static void subclass_window(HWND hwnd, message_handler* handler, UINT_PTR id_subclass = {}, DWORD_PTR ref_data = {}) {
    if (!SetWindowSubclass(hwnd, wndkit::dispatcher::sub_class_proc, id_subclass, ref_data))
      throw std::system_error(static_cast<int>(GetLastError()), std::system_category());

    attach_window(hwnd, handler);
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
      else if (!ret)
        return static_cast<int>(msg.wParam);

      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }
  }

  static LRESULT CALLBACK def_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    auto result = call_handler(hwnd, msg, wparam, lparam);
    return result.value_or(DefWindowProcW(hwnd, msg, wparam, lparam));
  }

  static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    auto result = call_handler(hwnd, msg, wparam, lparam);
    return result.value_or(FALSE);
  }

  static LRESULT CALLBACK sub_class_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, [[maybe_unused]] UINT_PTR id_subclass, [[maybe_unused]] DWORD_PTR ref_data) {
    auto result = call_handler(hwnd, msg, wparam, lparam);
    return result.value_or(DefSubclassProc(hwnd, msg, wparam, lparam));
  }

private:
  static auto& handlers() {
    thread_local static std::unordered_map<HWND, message_handler*> handlers_;
    return handlers_;
  }

public:
  static void attach_window(HWND hwnd, message_handler* handler) {
    assert(hwnd);
    assert(handler);

    auto inserted = handlers().insert({hwnd, handler});
    assert(inserted.second);
  }

private:
  static std::optional<LRESULT> call_handler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_NCCREATE) {
      auto create_struct = reinterpret_cast<CREATESTRUCTW*>(lparam);
      auto create_params = reinterpret_cast<message_handler::create_params*>(create_struct->lpCreateParams);
      create_struct->lpCreateParams = create_params->original_create_params;

      auto ret = create_params->handler->call_handler(hwnd, msg, wparam, lparam);
      if (ret.value_or(true))
        handlers().insert({hwnd, create_params->handler});

      return ret;
    } else {
      auto match = handlers().find(hwnd);
      if (match == handlers().end()) {
        return std::nullopt;
      } else {
        auto ret = match->second->call_handler(hwnd, msg, wparam, lparam);

        if (msg == WM_NCDESTROY)
          handlers().erase(match);

        return ret;
      }
    }
  }
};

}
