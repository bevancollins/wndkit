// Copyright (c) 2025 Bevan Collins
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include <windows.h>
#include <system_error>
#include "wndkit/message_handler.hpp"

namespace wndkit {

class dispatcher {
public:
  /*
     Retrieves a reference to the thread-local `message_handler`.
     The `message_handler` instance is used to register handlers for various Windows messages.
  */
  static message_handler& handler() {
    thread_local static message_handler handler_;
    return handler_;
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
    auto result = handler().call_handler(hwnd, msg, wparam, lparam);
    return result.value_or(DefWindowProcW(hwnd, msg, wparam, lparam));
  }

  static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    auto result = handler().call_handler(hwnd, msg, wparam, lparam);
    return result.value_or(FALSE);
  }
};

}
