#pragma once

#include <windows.h>
#include <shellscalingapi.h>
#include <wil/resource.h>
#include <wndkit/dispatcher.hpp>
#include <wndkit/message_handler.hpp>
#include "layout.hpp"

namespace wndkit::widgets {

class top_level_window {
public:
  top_level_window(box_layout::orientation orientation = box_layout::orientation::vertical)
    : layout_(message_handler_, orientation) {

    message_handler_
      .on_message<WM_CREATE>([this](HWND hwnd, const create_params& params) {
        on_create(hwnd, params);
      })
      .on_message<WM_DPICHANGED>([this](HWND hwnd, const auto& params) {
        on_dpi_changed(hwnd, params);
      })
      .on_message<WM_SIZE>([this](HWND hwnd, const auto& params) {
        on_size(hwnd, params);
      })
      .on_message<WM_CLOSE>([this](HWND hwnd, const auto& params) {
        on_close(hwnd, params);
      })
      .on_message<WM_CTLCOLORSTATIC>([this](HWND hwnd, const auto& params) {
        return on_ctlcolorstatic(hwnd, params);
      })
    ;
  }

  template<typename... Args>
  auto create(Args&&... args) {
    return wndkit::dispatcher::create_window(&message_handler_, std::forward<Args>(args)...);
  }

  box_layout& layout() noexcept { return layout_; }

protected:
  virtual void on_create(HWND hwnd, const create_params&) {
    refresh_font(hwnd, GetDpiForWindow(hwnd));
  }

  virtual void on_dpi_changed(HWND hwnd, const dpichanged_params& params) {
    refresh_font(hwnd, params.dpi_x());

    auto suggested = params.suggested_rect();
    SetWindowPos(
        hwnd, nullptr,
        suggested->left, suggested->top,
        suggested->right - suggested->left,
        suggested->bottom - suggested->top,
        SWP_NOZORDER | SWP_NOACTIVATE);
  }

  virtual void on_size(HWND hwnd, const wndkit::size_params& params) {
    layout_.resize(params.sz());
  }

  virtual void on_close(HWND hwnd, [[maybe_unused]] const wndkit::close_params& params) {
    DestroyWindow(hwnd);
  }

  virtual LRESULT on_ctlcolorstatic(HWND hwnd, const wndkit::ctlcolorstatic_params& params) {
    SetBkMode(params.hdc(), TRANSPARENT);
    return reinterpret_cast<LRESULT>(GetSysColorBrush(COLOR_WINDOW));
  }

  static wil::unique_hfont get_default_ui_font(UINT dpi) {
    NONCLIENTMETRICSW ncm{};
    ncm.cbSize = sizeof(ncm);

    if (SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0, dpi))
      return wil::unique_hfont(CreateFontIndirectW(&ncm.lfMessageFont));

    // Fallback if that fails
    LOGFONTW lf{};
    GetObjectW(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
    lf.lfUnderline = TRUE;
    return wil::unique_hfont(CreateFontIndirectW(&lf));
  }

  void refresh_font(HWND hwnd, UINT dpi) {
    font_ = get_default_ui_font(dpi);
    SendMessageW(hwnd, WM_SETFONT, reinterpret_cast<WPARAM>(font_.get()), TRUE);
    EnumChildWindows(hwnd, [](HWND child, LPARAM font_param) -> BOOL {
      SendMessageW(child, WM_SETFONT, font_param, TRUE);
      return TRUE;
    }, reinterpret_cast<LPARAM>(font_.get()));
  }

  wil::unique_hfont font_;
  wndkit::message_handler message_handler_;
  box_layout layout_;
};

}
