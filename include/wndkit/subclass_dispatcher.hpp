// Copyright (c) 2025 Bevan Collins
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include <windows.h>
#include <mutex>
#include <system_error>
#include <unordered_map>
#include "wndkit/message_handler.hpp"

namespace wndkit {

class subclass_dispatcher {
public:
  subclass_dispatcher() = default;

  virtual ~subclass_dispatcher() {
    for (const auto& [hwnd, original_proc] : subclasses_) {
      if (hwnd && original_proc) {
        SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(original_proc));
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
      }
    }
  }

  message_handler& handler() {
    return handler_;
  }

  void subclass(HWND hwnd) {
    WNDPROC original_proc = reinterpret_cast<WNDPROC>(
      SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&subclass_dispatcher::window_proc))
    );
    if (!original_proc)
      throw std::system_error(static_cast<int>(GetLastError()), std::system_category());

    SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    std::scoped_lock lock(mutex_);
    subclasses_.emplace(hwnd, original_proc);
  }

  void unsubclass(HWND hwnd) {
    std::scoped_lock lock(mutex_);
    auto it = subclasses_.find(hwnd);
    if (it != subclasses_.end()) {
      SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(it->second));
      SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
      subclasses_.erase(it);
    }
  }

private:
  static LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    auto self = reinterpret_cast<subclass_dispatcher*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (self) {
      if (auto result = self->handler_.call_handler(hwnd, msg, wparam, lparam))
        return *result;

      WNDPROC wndproc{};
      {
        std::scoped_lock lock(self->mutex_);
        auto it = self->subclasses_.find(hwnd);
        if (it != self->subclasses_.end())
          wndproc = it->second;
      }

      if (msg == WM_NCDESTROY)
        self->unsubclass(hwnd);

      if (wndproc)
        return CallWindowProcW(wndproc, hwnd, msg, wparam, lparam);
    }
    return DefWindowProcW(hwnd, msg, wparam, lparam);
  }

  message_handler handler_;
  std::unordered_map<HWND, WNDPROC> subclasses_;
  std::mutex mutex_;
};

}
