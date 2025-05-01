// Copyright (c) 2025 Bevan Collins
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include <windows.h>
#include <commctrl.h>
#include <mutex>
#include <system_error>
#include <unordered_map>
#include "wndkit/message_handler.hpp"

namespace wndkit {

class commctrl_subclass_dispatcher {
public:
  commctrl_subclass_dispatcher() = default;

  virtual ~commctrl_subclass_dispatcher() {
    for (auto& [hwnd, id] : ids_)
      RemoveWindowSubclass(hwnd, subclass_proc, id);
  }

  message_handler& handler() {
    return handler_;
  }

  void subclass(HWND hwnd) noexcept(false) {
    std::scoped_lock lock(mutex_);
    if (ids_.contains(hwnd))
      throw std::logic_error("window already subclassed");

    auto id = next_id();
    if (!SetWindowSubclass(hwnd, subclass_proc, id, reinterpret_cast<DWORD_PTR>(this)))
      throw std::system_error(static_cast<int>(GetLastError()), std::system_category());

    ids_[hwnd] = id;
  }

  void unsubclass(HWND hwnd) noexcept(false) {
    std::scoped_lock lock(mutex_);
    auto it = ids_.find(hwnd);
    if (it != ids_.end()) {
      if (!RemoveWindowSubclass(hwnd, subclass_proc, it->second))
        throw std::system_error(static_cast<int>(GetLastError()), std::system_category());

      ids_.erase(it);
    }
  }

private:
  static LRESULT CALLBACK subclass_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, [[maybe_unused]] UINT_PTR id_subclass, DWORD_PTR ref_data) {
    auto self = reinterpret_cast<commctrl_subclass_dispatcher*>(ref_data);

    if (msg == WM_NCDESTROY) {
      std::scoped_lock lock(self->mutex_);
      self->ids_.erase(hwnd);
    }

    auto result = self->handler_.call_handler(hwnd, msg, wparam, lparam);
    if (result)
      return *result;
    else
      return DefSubclassProc(hwnd, msg, wparam, lparam);
  }

  static UINT next_id() noexcept {
    static UINT id{};
    return id++;
  }

  message_handler handler_;
  std::unordered_map<HWND, UINT> ids_;
  std::mutex mutex_;
};

}
