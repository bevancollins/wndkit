#include <windows.h>
#include <commctrl.h>
#include <string>
#include <wndkit/dispatcher.hpp>
#include <wndkit/message_handler.hpp>

#define IDC_BUTTON 1001

class ticker : public wndkit::message_handler {
public:
  void create(HWND parent, int x, int y, int width, int height, HINSTANCE instance) {
    auto hwnd = CreateWindowW(
        WC_STATICW, {},
        WS_CHILD | WS_VISIBLE,
        x, y, width, height,
        parent, {}, instance, {});
    wndkit::dispatcher::subclass_window(hwnd, this);

    SetTimer(hwnd, 1, 1000, nullptr);

    on_message<WM_TIMER>([this](HWND hwnd, [[maybe_unused]] const wndkit::timer_params&) {
      SetWindowTextW(hwnd, std::to_wstring(++seconds_).c_str());
    });
  }

private:
  int seconds_{};
};

class example_window : public wndkit::message_handler {
public:
  example_window() {
    on_message<WM_CREATE>([this](HWND hwnd, const wndkit::create_params& params) {
      on_create(hwnd, params);
    });

    on_message_invoke<WM_CLOSE>(DestroyWindow);
    on_message_invoke<WM_DESTROY>(PostQuitMessage, 0);
    on_command_invoke(IDC_BUTTON, MessageBoxW, L"Button clicked!", L"Info", MB_OK);
  }

private:
  void on_create(HWND hwnd, const wndkit::create_params& params) {
    CreateWindowW(
        WC_BUTTONW, L"Click Me",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        50, 50, 150, 40,
        hwnd, (HMENU)IDC_BUTTON, params.createstruct()->hInstance, {});

    ticker_.create(hwnd, 50, 100, 50, 20, params.createstruct()->hInstance);
  }

  ticker ticker_;
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int) {
  INITCOMMONCONTROLSEX init_cc{
    .dwSize = sizeof(init_cc),
    .dwICC = ICC_STANDARD_CLASSES,
  };
  InitCommonControlsEx(&init_cc);

  WNDCLASSW wc{
    .lpfnWndProc = wndkit::dispatcher::def_window_proc,
    .hInstance = hInstance,
    .hCursor = LoadCursor(nullptr, IDC_ARROW),
    .hbrBackground = (HBRUSH)(COLOR_WINDOW+1),
    .lpszClassName = L"wndkit_example",
  };
  RegisterClassW(&wc);

  example_window window;
  window.create(
      0, L"wndkit_example", L"Example",
      WS_OVERLAPPEDWINDOW | WS_VISIBLE,
      CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
      nullptr, nullptr, hInstance, nullptr);

  return wndkit::dispatcher::run();
}
