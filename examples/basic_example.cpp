#include <windows.h>
#include <commctrl.h>
#include <string>
#include <utility>
#include <wndkit/dispatcher.hpp>
#include <wndkit/message_handler.hpp>

#define IDC_START_BUTTON 1001
#define IDC_STOP_BUTTON  1002

class ticker {
public:
  void create(HWND parent, int x, int y, int width, int height, HINSTANCE instance) {
    hwnd_ = wndkit::dispatcher::create_subclass_window(message_handler_,
        {}, WC_STATICW, {},
        WS_CHILD | WS_VISIBLE,
        x, y, width, height,
        parent, {}, instance, {});

    message_handler_.on_message<WM_TIMER>([this](HWND, const auto&) {
      tick();
    });
  }

  void start() {
    SetTimer(hwnd_, 1, 1000, nullptr);
  }

  void stop() {
    KillTimer(hwnd_, 1);
  }

private:
  void tick() {
    seconds_++;
    SetWindowTextW(hwnd_, std::to_wstring(seconds_).c_str());
  }

  HWND hwnd_{};
  wndkit::message_handler message_handler_;
  int seconds_{};
};

class example_window {
public:
  example_window() {
    message_handler_
      .on_message<WM_CREATE>([this](HWND hwnd, const auto& params) {
        on_create(hwnd, params);
      })
      .on_message_invoke<WM_CLOSE>(DestroyWindow)
      .on_message_invoke<WM_DESTROY>(PostQuitMessage, 0)
      .on_command_invoke(IDC_START_BUTTON, [this]() {
        ticker_.start();
      })
      .on_command_invoke(IDC_STOP_BUTTON, [this]() {
        ticker_.stop();
      });
  }

  template<typename... Args>
  decltype(auto) create(Args&&... args) {
    return wndkit::dispatcher::create_window(message_handler_, std::forward<Args>(args)...);
  }

private:
  void on_create(HWND hwnd, const wndkit::create_params& params) {
    CreateWindowW(
        WC_BUTTONW, L"Start",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        50, 50, 150, 20,
        hwnd, (HMENU)IDC_START_BUTTON, params.createstruct()->hInstance, {});

    CreateWindowW(
        WC_BUTTONW, L"Stop",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        50, 70, 150, 20,
        hwnd, (HMENU)IDC_STOP_BUTTON, params.createstruct()->hInstance, {});

    ticker_.create(hwnd, 50, 100, 50, 20, params.createstruct()->hInstance);
  }

  ticker ticker_;
  wndkit::message_handler message_handler_;
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int) {
  INITCOMMONCONTROLSEX init_cc{
    .dwSize = sizeof(init_cc),
    .dwICC = ICC_STANDARD_CLASSES,
  };
  InitCommonControlsEx(&init_cc);

  WNDCLASSW wc{
    .lpfnWndProc = wndkit::dispatcher::window_proc,
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
