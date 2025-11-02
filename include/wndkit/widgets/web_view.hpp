#pragma once

#include <windows.h>
#include <string>
#include <system_error>
#include <wil/resource.h>
#include <wil/com.h>
#include <wrl.h>
#include <WebView2.h>
#include <wndkit/dispatcher.hpp>
#include <wndkit/message_handler.hpp>

namespace wndkit::widgets {

class web_view {
public:
  web_view() = default;

  // register the window class once (call before creating any widgets)
  [[nodiscard]] static ATOM register_class(HINSTANCE instance) {
    WNDCLASSW wc{};
    wc.lpfnWndProc   = wndkit::dispatcher::window_proc;
    wc.hCursor       = LoadCursorW(nullptr, reinterpret_cast<LPCWSTR>(IDC_ARROW));
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.hInstance     = instance;
    wc.lpszClassName = L"wndkit_webview";

    auto atom = RegisterClassW(&wc);
    if (!atom)
      throw std::system_error(static_cast<int>(GetLastError()), std::system_category());

    return atom;
  }

  struct settings_t {
    bool is_script_enabled{true};
    bool are_default_script_dialogs_enabled{true};
    bool is_web_message_enabled{true};
    bool are_default_context_menus_enabled{false};
  };
  settings_t& settings() {
    return settings_;
  }

  void create(HWND parent, int x, int y, int width, int height, ATOM atom) {
    auto hwnd = wndkit::dispatcher::create_window(&message_handler_,
      0,
      reinterpret_cast<LPCWSTR>(static_cast<WORD>(atom)),
      nullptr,
      WS_CHILD | WS_VISIBLE,
      x, y, width, height,
      parent, nullptr,
      nullptr, nullptr);
    if (!hwnd)
      throw std::system_error(static_cast<int>(GetLastError()), std::system_category());

    message_handler_
      .on_message<WM_SIZE>([this](HWND hwnd, const auto&) { on_size(hwnd); })
      .on_message_invoke<WM_DESTROY>([this]() { on_destroy(); });

    init_webview2_async(hwnd);
  }

  void navigate(const std::wstring& url) {
    if (webview2_)
      webview2_->Navigate(url.c_str());
    else
      // store pending url in case webview not ready yet
      pending_url_ = url;
  }

  void reload() {
    if (webview2_)
      webview2_->Reload();
  }

  void execute_script(const std::wstring& script) {
    if (webview2_)
      webview2_->ExecuteScript(script.c_str(), nullptr);
  }

private:
  void init_webview2_async(HWND hwnd) {
    Microsoft::WRL::ComPtr<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler> cb_env = Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
      [this, hwnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
        if (FAILED(result) || env == nullptr)
          return S_OK; // fail silently for now

        // create controller attached to our hwnd_
        auto env_ptr = Microsoft::WRL::ComPtr<ICoreWebView2Environment>(env);
        env_ptr->CreateCoreWebView2Controller(hwnd,
          Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
            [this, hwnd](HRESULT result2, ICoreWebView2Controller* controller) -> HRESULT {
              if (FAILED(result2) || controller == nullptr)
                return S_OK; // failed to create controller

              webview_controller_ = controller;

              auto hr = webview_controller_->get_CoreWebView2(&webview2_);
              if (FAILED(hr) || !webview2_) {
                webview_controller_.Reset();
                return S_OK;
              }

              // enable default settings if desired
              Microsoft::WRL::ComPtr<ICoreWebView2Settings> settings;
              if (SUCCEEDED(webview2_->get_Settings(&settings)) && settings) {
                settings->put_IsScriptEnabled               (settings_.is_script_enabled ? TRUE : FALSE);
                settings->put_AreDefaultScriptDialogsEnabled(settings_.are_default_script_dialogs_enabled ? TRUE : FALSE);
                settings->put_IsWebMessageEnabled           (settings_.is_web_message_enabled ? TRUE : FALSE);
                settings->put_AreDefaultContextMenusEnabled (settings_.are_default_context_menus_enabled ? TRUE : FALSE);
              }

              on_size(hwnd);

              // navigate pending url if present
              if (!pending_url_.empty())
                webview2_->Navigate(pending_url_.c_str());

              return S_OK;
            }).Get());
        return S_OK;
      }).Get();

    CreateCoreWebView2Environment(cb_env.Get());
  }

  void on_size(HWND hwnd) {
    if (webview_controller_) {
      RECT rc{};
      GetClientRect(hwnd, &rc);
      webview_controller_->put_Bounds(rc);
    }
  }

  void on_destroy() {
    // release WebView2 COM objects first (controller must be released before window destroyed)
    webview2_.Reset();
    webview_controller_.Reset();
  }

  wndkit::message_handler message_handler_;
  std::wstring pending_url_;
  settings_t settings_;
  Microsoft::WRL::ComPtr<ICoreWebView2Controller> webview_controller_;
  Microsoft::WRL::ComPtr<ICoreWebView2> webview2_;
};

}
