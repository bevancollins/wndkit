#pragma once

#include <windows.h>
#include <wndkit/dispatcher.hpp>
#include <wndkit/message_handler.hpp>

namespace wndkit::widgets {

class top_level_window {
public:
  top_level_window() {
    message_handler_
      .on_message<WM_DPICHANGED>([this](HWND hwnd, const auto& params) {
        on_dpi_changed(hwnd, params);
      })
      .on_message<WM_CLOSE>([this](HWND hwnd, const auto& params) {
        on_close(hwnd, params);
      })
      .on_message<WM_CTLCOLORSTATIC>([this](HWND hwnd, const auto& params) {
        return on_ctl_color_static(hwnd, params);
      })
    ;
  }

  template<typename... Args>
  auto create(Args&&... args) {
    return wndkit::dispatcher::create_window(&message_handler_, std::forward<Args>(args)...);
  }

protected:
  virtual void on_dpi_changed(HWND hwnd, const dpichanged_params& params) {
    auto suggested = params.suggested_rect();
    SetWindowPos(
        hwnd, nullptr,
        suggested->left, suggested->top,
        suggested->right - suggested->left,
        suggested->bottom - suggested->top,
        SWP_NOZORDER | SWP_NOACTIVATE);
  }

  virtual void on_close(HWND hwnd, [[maybe_unused]] const wndkit::close_params& params) {
    DestroyWindow(hwnd);
  }

  virtual LRESULT on_ctl_color_static(HWND hwnd, const wndkit::ctlcolorstatic_params& params) {
    SetBkMode(params.hdc(), TRANSPARENT);
    return reinterpret_cast<LRESULT>(GetSysColorBrush(COLOR_WINDOW));
  }

  wndkit::message_handler message_handler_;
};

}
