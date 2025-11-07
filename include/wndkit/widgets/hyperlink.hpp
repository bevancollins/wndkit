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

  static constexpr const wchar_t* class_name() {
    return L"wndkit_hyperlink";
  }

  HWND create(HWND parent, int x, int y, int width, int height, HINSTANCE instance) {
    hwnd_ = wndkit::dispatcher::create_window(&message_handler_,
      0,
      class_name(),
      nullptr,
      WS_CHILD | WS_VISIBLE,
      x, y, width, height,
      parent, nullptr,
      instance, nullptr);
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
      })
    ;

    return hwnd_;
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

    if (text_.empty() || text_ == url)
      set_text(url_);
  }

  auto url() const {
    return url_;
  }

  // Static method to register the custom window class
  static ATOM register_class(HINSTANCE instance) {
    WNDCLASSW wc{};
    wc.lpfnWndProc   = wndkit::dispatcher::window_proc;
    wc.hCursor       = LoadCursorW(nullptr, reinterpret_cast<LPCWSTR>(IDC_HAND));
    wc.hInstance     = instance;
    wc.lpszClassName = class_name();

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
    auto hdc = wil::BeginPaint(hwnd_, &ps);

    // Ask parent for colors
    wndkit::ctlcolorstatic_params ctlcolorstatic_params;
    ctlcolorstatic_params.set_hdc(hdc.get());
    ctlcolorstatic_params.set_hctl(hwnd_);

    auto background = reinterpret_cast<HBRUSH>(SendMessageW(GetParent(hwnd_), WM_CTLCOLORSTATIC, ctlcolorstatic_params.wparam, ctlcolorstatic_params.lparam));
    if (background)
      FillRect(hdc.get(), &ps.rcPaint, background);
    else
      FillRect(hdc.get(), &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

    auto old_font = wil::SelectObject(hdc.get(), font_);

    COLORREF color{};
    if (hovered_)
      color = hover_color_;
    else if (visited_)
      color = visited_color_;
    else
      color = unvisited_color_;

    SetTextColor(hdc.get(), color);
    std::wstring& text = text_.empty() ? url_ : text_;

    DrawTextW(hdc.get(), text.c_str(), static_cast<int>(text.length()), &ps.rcPaint, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
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
