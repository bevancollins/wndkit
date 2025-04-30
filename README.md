# wndkit

**wndkit** is a modern C++20 Win32 message routing library designed to make message handling intuitive, type-safe, and fun. It wraps raw Win32 message dispatch into clean, composable APIs.

## ✨ Features

- Type-safe handlers for Win32 messages.
- Support for `boost::signals2` via `*_invoke` variants.
- Sticks to it's lane, only does message routing. This is a library not a framework.

## 🚀 Getting Started

```cpp
#include <windows.h>
#include <wndkit/dispatcher.hpp>

#define IDC_BUTTON1 1001

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
  WNDCLASS wc{};
  wc.lpfnWndProc = wndkit::dispatcher::def_window_proc;
  wc.hInstance = hInstance;
  wc.lpszClassName = L"DemoWndClass";
  RegisterClassW(&wc);

  HWND hwnd = CreateWindowExW(
    0, L"DemoWndClass", L"Example",
    WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
    nullptr, nullptr, hInstance, nullptr
  );

  WORD button_id = 1001;

  HWND button = CreateWindowW(
    WC_BUTTON, L"Click Me",
    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    50, 50, 150, 40,
    hwnd, (HMENU)IDC_BUTTON1, hInstance, nullptr
  );

  wndkit::dispatcher::handler()
    .on_message_invoke<WM_CLOSE>(DestroyWindow)
    .on_message_invoke<WM_DESTROY>(PostQuitMessage, 0)
    .on_command_invoke(IDC_BUTTON1, MessageBoxW, L"Button clicked!", L"Info", MB_OK)
   ;

  return wndkit::dispatcher::run();
}
