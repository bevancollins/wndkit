#include <windows.h>
#include <commctrl.h>
#include <string>
#include <wndkit/dispatcher.hpp>
#include <wndkit/message_handler.hpp>
#include <wndkit/widgets/main_window.hpp>
#include <wndkit/widgets/vbox_layout.hpp>

#define IDC_START_BUTTON 1001
#define IDC_STOP_BUTTON  1002

class ticker {
public:
  HWND create(HWND parent, int x, int y, int width, int height, HINSTANCE instance) {
    hwnd_ = wndkit::dispatcher::create_subclass_window(&message_handler_,
        {}, WC_STATICW, {},
        WS_CHILD | WS_VISIBLE,
        x, y, width, height,
        parent, {}, instance, {});

    message_handler_
      .on_message<WM_TIMER>([this](HWND, const auto&) {
        tick();
      })
    ;

    return hwnd_;
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

class example_window : public wndkit::widgets::main_window {
public:
  example_window() {
    message_handler_
      .on_command_invoke(IDC_START_BUTTON, [this]() {
        ticker_.start();
      })
      .on_command_invoke(IDC_STOP_BUTTON, [this]() {
        ticker_.stop();
      })
    ;


    set_layout(std::make_unique<wndkit::widgets::vbox_layout>());
  }

private:
  void on_create(HWND hwnd, const wndkit::create_params& params) override {
    layout()
      .add_widget(CreateWindowW(
        WC_BUTTONW, L"Start",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 0, 0, 0,
        hwnd, (HMENU)IDC_START_BUTTON, params.createstruct()->hInstance, {}))

      .add_widget(CreateWindowW(
        WC_BUTTONW, L"Stop",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 0, 0, 0,
        hwnd, (HMENU)IDC_STOP_BUTTON, params.createstruct()->hInstance, {}))
      .add_widget(ticker_.create(hwnd, 0, 0, 0, 0, params.createstruct()->hInstance))
    ;

    wndkit::widgets::main_window::on_create(hwnd, params);
  }

  ticker ticker_;
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int) {
  SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  INITCOMMONCONTROLSEX init_cc{
    .dwSize = sizeof(init_cc),
    .dwICC = ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES,
  };
  InitCommonControlsEx(&init_cc);

  WNDCLASSW wc{
    .lpfnWndProc = wndkit::dispatcher::window_proc,
    .hInstance = hInstance,
    .hCursor = LoadCursor(nullptr, IDC_ARROW),
    .hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW+1),
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
