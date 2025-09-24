#include <windows.h>
#include <commctrl.h>
#include <string>
#include <wndkit/dispatcher.hpp>

#define IDC_BUTTON 1001
#define IDC_CLOCK 1002

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int) {
  WNDCLASSW wc{
    .lpfnWndProc = wndkit::dispatcher::def_window_proc,
    .hInstance = hInstance,
    .hCursor = LoadCursor(nullptr, IDC_ARROW),
    .hbrBackground = (HBRUSH)(COLOR_WINDOW+1),
    .lpszClassName = L"wndkit_example",
  };
  RegisterClassW(&wc);

  auto hwnd = CreateWindowExW(
      0, L"wndkit_example", L"Example",
      WS_OVERLAPPEDWINDOW | WS_VISIBLE,
      CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
      nullptr, nullptr, hInstance, nullptr);

  CreateWindowW(
      WC_BUTTONW, L"Click Me",
      WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
      50, 50, 150, 40,
      hwnd, (HMENU)IDC_BUTTON, hInstance, nullptr);

  auto clock = 0;
  auto clock_hwnd = CreateWindowW(
      WC_STATICW, std::to_wstring(clock).c_str(),
      WS_CHILD | WS_VISIBLE,
      50, 100, 50, 20,
      hwnd, (HMENU)IDC_CLOCK, hInstance, nullptr);

  SetTimer(hwnd, 1, 1000, nullptr);

  wndkit::dispatcher::handler()
    .on_message_invoke<WM_CLOSE>(DestroyWindow)
    .on_message_invoke<WM_DESTROY>(PostQuitMessage, 0)
    .on_command_invoke(IDC_BUTTON, MessageBoxW, L"Button clicked!", L"Info", MB_OK)
    .on_message<WM_TIMER>([clock_hwnd, &clock]([[maybe_unused]] HWND hwnd, [[maybe_unused]] const wndkit::timer_params&) {
      SetWindowTextW(clock_hwnd, std::to_wstring(++clock).c_str());
    })
   ;

  return wndkit::dispatcher::run();
}
