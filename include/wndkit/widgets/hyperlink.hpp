#pragma once

#include <windows.h>
#include <shellapi.h>
#include <wil/resource.h>
#include <string>
#include <system_error>
#include <wndkit/dispatcher.hpp>
#include <wndkit/message_handler.hpp>

namespace wndkit::widgets {

class hyperlink {
public:
  hyperlink() = default;

  void create(HWND parent, int x, int y, int width, int height, ATOM atom) {
    font_ = create_font();

    hwnd_ = wndkit::dispatcher::create_window(&message_handler_,
      0,
      reinterpret_cast<LPCWSTR>(static_cast<WORD>(atom)),
      nullptr,
      WS_CHILD | WS_VISIBLE,
      x, y, width, height,
      parent, nullptr,
      nullptr, nullptr);
    if (!hwnd_)
      throw std::system_error(static_cast<int>(GetLastError()), std::system_category());

    message_handler_
      .on_message_invoke<WM_PAINT>([this]()      { on_paint(); })
      .on_message_invoke<WM_MOUSEMOVE>([this]()  { on_mouse_move(); })
      .on_message_invoke<WM_MOUSELEAVE>([this]() { on_mouse_leave(); })
      .on_message_invoke<WM_LBUTTONDOWN>([this](){ on_button_down(); })
      .on_message_invoke<WM_LBUTTONUP>([this]()  { on_button_up(); });
  }

  void set_url(const auto& url) {
    url_ = url;
    if (hwnd_) InvalidateRect(hwnd_, nullptr, TRUE);
  }

  std::wstring url() const {
    return url_;
  }

  // Static method to register the custom window class
  [[nodiscard]] static ATOM register_class(HINSTANCE instance) {
    WNDCLASSW wc{};
    wc.lpfnWndProc   = wndkit::dispatcher::window_proc;
    wc.hCursor       = LoadCursorW(nullptr, reinterpret_cast<LPCWSTR>(IDC_HAND));
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.hInstance     = instance;
    wc.lpszClassName = L"wndkit_hyperlink";

    auto atom = RegisterClassW(&wc);
    if (!atom)
      throw std::system_error(static_cast<int>(GetLastError()), std::system_category());

    return atom;
  }

private:
  void on_paint() {
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hwnd_, &ps);

    RECT rc{};
    GetClientRect(hwnd_, &rc);

    auto old_font = SelectObject(hdc, font_.get());
    SetBkMode(hdc, TRANSPARENT);

    COLORREF color{};
    if (hovered_)
      color = hover_color_;
    else if (visited_)
      color = visited_color_;
    else
      color = unvisited_color_;

    SetTextColor(hdc, color);
    DrawTextW(hdc, url_.c_str(), static_cast<int>(url_.length()), &rc, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

    SelectObject(hdc, old_font);
    EndPaint(hwnd_, &ps);
  }

  void on_mouse_move() {
    if (!hovered_) {
      TRACKMOUSEEVENT tme{sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd_, 0};
      TrackMouseEvent(&tme);

      hovered_ = true;
      InvalidateRect(hwnd_, nullptr, TRUE);
    }
  }

  void on_mouse_leave() {
    hovered_ = false;
    button_down_ = false;
    InvalidateRect(hwnd_, nullptr, TRUE);
  }

  void on_button_down() {
    button_down_ = true;
  }

  void on_button_up() {
    if (button_down_) {
      button_down_ = false;
      visited_ = true;
      InvalidateRect(hwnd_, nullptr, TRUE);

      if (!url_.empty())
        ShellExecuteW(nullptr, L"open", url_.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
  }

  wil::unique_hfont create_font() {
    LOGFONTW lf{};
    GetObjectW(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
    lf.lfUnderline = TRUE;
    return wil::unique_hfont(CreateFontIndirectW(&lf));
  }

  static constexpr COLORREF unvisited_color_ = RGB(0x00, 0x00, 0xEE);
  static constexpr COLORREF visited_color_   = RGB(0x55, 0x1A, 0x8B);
  static constexpr COLORREF hover_color_     = RGB(0xFF, 0x00, 0x00);

  bool visited_{};
  bool hovered_{};
  bool button_down_{};

  wndkit::message_handler message_handler_;
  wil::unique_hfont font_;

  HWND hwnd_{};
  std::wstring url_;
};

}
