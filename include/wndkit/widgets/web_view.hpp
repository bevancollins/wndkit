#pragma once

#include <windows.h>
#include <memory>
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
  web_view() {
    settings_ = std::make_shared<unbound_settings>();
  }

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

  class settings_t {
  public:
    virtual bool is_script_enabled() const = 0;
    virtual void set_script_enabled(bool f) = 0;
    virtual bool are_default_script_dialogs_enabled() const = 0;
    virtual void set_default_script_dialogs_enabled(bool f) = 0;
    virtual bool is_web_message_enabled() const = 0;
    virtual void set_web_message_enabled(bool f) = 0;
    virtual bool are_default_context_menus_enabled() const = 0;
    virtual void set_default_context_menus_enabled(bool f) = 0;
  };

  class unbound_settings : public settings_t {
  public:
    bool is_script_enabled() const override { return is_script_enabled_; }
    void set_script_enabled(bool f) override { is_script_enabled_ = f; }
    bool are_default_script_dialogs_enabled() const override { return are_default_script_dialogs_enabled_; }
    void set_default_script_dialogs_enabled(bool f) override { are_default_script_dialogs_enabled_ = f; }
    bool is_web_message_enabled() const override { return is_web_message_enabled_; }
    void set_web_message_enabled(bool f) override { is_web_message_enabled_ = f; }
    bool are_default_context_menus_enabled() const override { return are_default_context_menus_enabled_; }
    void set_default_context_menus_enabled(bool f) override { are_default_context_menus_enabled_ = f; }

  private:
    bool is_script_enabled_{true};
    bool are_default_script_dialogs_enabled_{true};
    bool is_web_message_enabled_{true};
    bool are_default_context_menus_enabled_{false};
  };

  class bound_settings : public settings_t {
  public:
    bound_settings(Microsoft::WRL::ComPtr<ICoreWebView2Settings> web_view_settings) :
      web_view_settings_(web_view_settings) {
    }

    bool is_script_enabled() const override {
      BOOL f{};
      web_view_settings_->get_IsScriptEnabled(&f);
      return !!f;
    }

    void set_script_enabled(bool f) override {
      web_view_settings_->put_IsScriptEnabled(f ? TRUE : FALSE);
    }

    bool are_default_script_dialogs_enabled() const override {
      BOOL f{};
      web_view_settings_->get_AreDefaultScriptDialogsEnabled(&f);
      return !!f;
    }

    void set_default_script_dialogs_enabled(bool f) override {
      web_view_settings_->put_AreDefaultScriptDialogsEnabled(f ? TRUE : FALSE);
    }

    bool is_web_message_enabled() const override {
      BOOL f{};
      web_view_settings_->get_IsWebMessageEnabled(&f);
      return !!f;
    }

    void set_web_message_enabled(bool f) override {
      web_view_settings_->put_IsWebMessageEnabled(f ? TRUE : FALSE);
    }

    bool are_default_context_menus_enabled() const override {
      BOOL f{};
      web_view_settings_->get_AreDefaultContextMenusEnabled(&f);
      return !!f;
    }

    void set_default_context_menus_enabled(bool f) override {
      web_view_settings_->put_AreDefaultContextMenusEnabled(f ? TRUE : FALSE);
    }

  private:
    Microsoft::WRL::ComPtr<ICoreWebView2Settings> web_view_settings_;
  };

  auto settings() {
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
    if (webview_)
      webview_->Navigate(url.c_str());
    else
      // store pending url in case webview not ready yet
      pending_url_ = url;
  }

  void reload() {
    if (webview_)
      webview_->Reload();
  }

  void execute_script(const std::wstring& script) {
    if (webview_)
      webview_->ExecuteScript(script.c_str(), nullptr);
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

              auto hr = webview_controller_->get_CoreWebView2(&webview_);
              if (FAILED(hr) || !webview_) {
                webview_controller_.Reset();
                return S_OK;
              }

              // update the settings
              Microsoft::WRL::ComPtr<ICoreWebView2Settings> web_view_settings;
              if (SUCCEEDED(webview_->get_Settings(&web_view_settings)) && web_view_settings)
                replace_settings(std::make_shared<bound_settings>(web_view_settings));

              on_size(hwnd);

              // navigate pending url if present
              if (!pending_url_.empty())
                webview_->Navigate(pending_url_.c_str());

              return S_OK;
            }).Get());
        return S_OK;
      }).Get();

    CreateCoreWebView2Environment(cb_env.Get());
  }

  void replace_settings(std::shared_ptr<settings_t> new_settings) {
    new_settings->set_script_enabled                (settings_->is_script_enabled());
    new_settings->set_default_script_dialogs_enabled(settings_->are_default_script_dialogs_enabled());
    new_settings->set_web_message_enabled           (settings_->is_web_message_enabled());
    new_settings->set_default_context_menus_enabled (settings_->are_default_context_menus_enabled());

    settings_ = new_settings;
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
    settings_.reset();
    webview_.Reset();
    webview_controller_.Reset();
  }

  wndkit::message_handler message_handler_;
  std::wstring pending_url_;
  std::shared_ptr<settings_t> settings_;
  Microsoft::WRL::ComPtr<ICoreWebView2Controller> webview_controller_;
  Microsoft::WRL::ComPtr<ICoreWebView2> webview_;
};

}
