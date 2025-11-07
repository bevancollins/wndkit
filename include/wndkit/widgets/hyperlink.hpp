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
      .on_message_invoke<WM_LBUTTONUP>([this]()  { on_button_up(); })
      .on_message<WM_SETFONT>([this](HWND, const auto& params) {
        on_set_font(params);
      });
  }

  void set_text(const auto& text) {
    text_ = text;

    InvalidateRect(hwnd_, nullptr, TRUE);
  }

  auto text() const {
    return text_;
  }

  void set_url(const auto& url) {
    url_ = url;
    set_tooltip(url_);

    InvalidateRect(hwnd_, nullptr, TRUE);
  }

  auto url() const {
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
  void set_tooltip(const std::wstring& text) {
    // Create tooltip if needed
    if (!tooltip_hwnd_) {
      tooltip_hwnd_ = CreateWindowExW(
          WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr,
          WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
          CW_USEDEFAULT, CW_USEDEFAULT,
          CW_USEDEFAULT, CW_USEDEFAULT,
          hwnd_, nullptr,
          GetModuleHandleW(nullptr), nullptr);

      SetWindowPos(
          tooltip_hwnd_, HWND_TOPMOST, 0, 0, 0, 0,
          SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }

    TOOLINFOW ti{};
    ti.cbSize   = sizeof(ti);
    ti.uFlags   = TTF_IDISHWND | TTF_SUBCLASS;
    ti.hwnd     = hwnd_;
    ti.uId      = reinterpret_cast<UINT_PTR>(hwnd_);
    ti.lpszText = const_cast<wchar_t*>(text.c_str());

    SendMessageW(tooltip_hwnd_, TTM_ADDTOOLW, 0, reinterpret_cast<LPARAM>(&ti));
  }

  void on_paint() {
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hwnd_, &ps);

    RECT rc{};
    GetClientRect(hwnd_, &rc);

    auto old_font = wil::SelectObject(hdc, font_);
    SetBkMode(hdc, TRANSPARENT);

    COLORREF color{};
    if (hovered_)
      color = hover_color_;
    else if (visited_)
      color = visited_color_;
    else
      color = unvisited_color_;

    SetTextColor(hdc, color);
    std::wstring& text = text_.empty() ? url_ : text_;

    DrawTextW(hdc, text.c_str(), static_cast<int>(text.length()), &rc, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

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

  void on_set_font(const setfont_params& params) {
    font_ = params.hfont();
    if (params.should_redraw())
      InvalidateRect(hwnd_, nullptr, TRUE);
  }

  static constexpr COLORREF unvisited_color_ = RGB(0x00, 0x00, 0xEE);
  static constexpr COLORREF visited_color_   = RGB(0x55, 0x1A, 0x8B);
  static constexpr COLORREF hover_color_     = RGB(0xFF, 0x00, 0x00);

  bool visited_{};
  bool hovered_{};
  bool button_down_{};

  wndkit::message_handler message_handler_;

  HWND hwnd_{};
  HWND tooltip_hwnd_{};
  HFONT font_{};
  std::wstring text_;
  std::wstring url_;
};

}
