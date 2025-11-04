// Copyright (c) 2025 Bevan Collins
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include <windows.h>
#include <concepts>
#include <functional>
#include <unordered_map>
#include <optional>
#include <vector>
#include "message_filters.hpp"
#include "details/message_traits.hpp"
#include "details/notify_traits.hpp"

namespace wndkit {

class message_handler {
public:
  /*
     Registers a message handler for a specific Windows message.

     This is the primary interface for binding custom message handlers to
     Windows messages. It allows filtering based on message parameters and
     supports automatic type conversion to the appropriate parameter type
     for the message.

     Template Parameters:
       Msg    - The Windows message ID (e.g., WM_PAINT, WM_COMMAND).
       Handler - The handler to invoke when the message is received.
       Filter  - (Optional) A filter object to control handler invocation
                 based on message-specific criteria (defaults to `no_filter`).

     Requirements:
       - The `Handler` must accept parameters compatible with the message ID.
       - The parameter type for the message must be compatible with `message_params`.

     Returns:
       A reference to the `message_handler` to support method chaining.

     Example:
       handler.on_message<WM_PAINT>([](HWND hwnd, paint_params& params) {
         // handle WM_PAINT
       });
  */
  template<UINT Msg, typename Handler, typename Filter = no_filter>
  requires details::message_params_compatible<typename details::message_traits<Msg>::param_type>
  message_handler& on_message(Handler&& handler, Filter filter = {}) {
    using param_type = typename details::message_traits<Msg>::param_type;
    using handler_result_type = std::invoke_result_t<Handler, HWND, param_type&>;

    handlers_[Msg].push_back([handler = std::forward<Handler>(handler), filter = std::move(filter)](HWND hwnd, message_params& params) mutable -> std::optional<LRESULT> {
        auto& specialised_params = static_cast<param_type&>(params);

        if (!filter.matches(specialised_params))
          return std::nullopt;

        // invoke the handler
        if constexpr (std::is_same_v<handler_result_type, void>) {
          handler(hwnd, specialised_params);
          return 0; // auto-return 0 if handler returns void
        } else {
          return handler(hwnd, specialised_params);
        }
      });

    return *this;
  }

  /*
     Registers a message handler that invokes a handler without passing any message parameters.
     This is typically used when the handler is a signal, lambda, or function object
     that takes no arguments.

     An optional filter can be provided to control whether the handler should be invoked
     for a given message instance.

     Examples:
       on_message_invoke<WM_COMMAND>(some_signal);
       on_message_invoke<WM_TIMER>([] { update_timer(); });

     The handler must be invocable as: handler()
  */
  template<UINT Msg, typename Handler, typename Filter = no_filter>
  requires std::invocable<Handler>
  message_handler& on_message_invoke(Handler&& handler, Filter filter = {}) {
    return on_message<Msg>([handler = std::forward<Handler>(handler)](HWND, auto&) mutable {
        std::invoke(handler);
      }, std::move(filter));
  }

  /*
     Registers a message handler that invokes a handler with HWND as the first argument,
     followed by any additional user-supplied arguments.
  
     This overload allows you to bind any handler that takes an HWND and fixed arguments,
     and register it as the handler for a specific Windows message.
  
     Examples:
       on_message_invoke<WM_CLOSE>(DestroyWindow);
       on_message_invoke<WM_RBUTTONDOWN>(MessageBoxW, L"Right click!", L"Info", MB_OK);

     The handler must be invocable as: handler(HWND, Args...)
  */
  template<UINT Msg, typename Handler, typename... Args>
  requires std::invocable<Handler, HWND, Args...>
  message_handler& on_message_invoke(Handler&& handler, Args&&... args) {
    return on_message<Msg>(
        [handler = std::forward<Handler>(handler),
        ...captured_args = std::forward<Args>(args)](HWND hwnd, auto&) mutable {
      std::invoke(handler, hwnd, captured_args...);
    });
  }

  /*
     Registers a message handler that invokes a handler without passing HWND,
     but instead binds a fixed set of arguments to be passed to the handler.

     This overload is useful when the message handler does not need the HWND,
     and the handler is already fully bound with the necessary arguments.

     Examples:
       on_message_invoke<WM_DESTROY>(PostQuitMessage, 0);
       on_message_invoke<WM_USER>(some_function, 42, "data");

     The handler must be invocable as: handler(Args...)
  */
  template<UINT Msg, typename Handler, typename... Args>
  requires (sizeof...(Args) > 0) && std::invocable<Handler, Args...>
  message_handler& on_message_invoke(Handler&& handler, Args&&... args) {
    return on_message<Msg>(
        [handler = std::forward<Handler>(handler),
        ...captured_args = std::forward<Args>(args)](HWND, auto&) mutable {
      std::invoke(handler, captured_args...);
    });
  }

  /*
     Registers a handler for a specific notification message (WM_NOTIFY) that filters on a given
     notification code (Code) and control ID (control_id).

     The handler will be invoked with the HWND and a specialized parameter derived from the
     notification message. The parameter type is determined by the notification code using the
     `details::notify_traits` template.

     The handler must accept the HWND and a specialized notification parameter as arguments.

     Examples:
       on_notify<TVN_SELCHANGED>(IDC_METHODS, [](HWND hwnd, NMTREEVIEWW* params) {
         // Handle tree-view selection change.
       });
       on_notify<DL_DROPPED>(IDC_LIST, [](HWND hwnd, DRAGLISTINFO* params) {
         // Handle button click event.
       });

     If the handler's return type is `void`, the function automatically returns 0.
     Otherwise, it returns the result of the handler invocation.
  */
  template<UINT Code, typename Handler>
  message_handler& on_notify(UINT_PTR control_id, Handler&& handler) {
    return on_message<WM_NOTIFY>([handler = std::forward<Handler>(handler)](HWND hwnd, notify_params& params) mutable {
      using param_type = typename details::notify_traits<Code>::param_type;
      using handler_result_type = std::invoke_result_t<Handler, HWND, param_type&>;

      auto specialised_params = reinterpret_cast<param_type*>(params.nmhdr());

      if constexpr (std::is_same_v<handler_result_type, void>) {
        handler(hwnd, *specialised_params);
        return 0; // auto-return 0 if handler returns void
      } else {
        return handler(hwnd, *specialised_params);
      }
    }, notify_filter{.code = Code, .id_from = control_id});
  }

  /*
     Registers a handler for a specific WM_NOTIFY notification code and control ID,
     where the supplied handler does not require any parameters.

     This is a convenience overload of `on_notify` for cases where you only want
     to invoke a function or lambda without using the HWND or notification parameters.

     The handler must be invocable without arguments: `handler()`.

     Examples:
       on_notify_invoke<TVN_SELCHANGED>(IDC_TREE, [] {
         // Handle selection change with no parameters
       });

       on_notify_invoke<LVN_ITEMCHANGED>(IDC_LIST, SomeStaticFunction);

     Internally, this wraps the handler in a lambda that discards the HWND and
     notification parameters, and simply invokes the supplied handler.
  */
  template<UINT Code, typename Handler>
  requires std::invocable<Handler>
  message_handler& on_notify_invoke(UINT_PTR control_id, Handler&& handler) {
    return on_notify<Code>(control_id, [handler = std::forward<Handler>(handler)](HWND, auto&) mutable {
        std::invoke(handler);
      });
  }

  /*
     Registers a handler for a specific command control ID, typically triggered by menu items,
     buttons, or other controls sending WM_COMMAND messages.

     Internally, this uses a filter to match only WM_COMMAND messages with the given control ID.

     Example:
       on_command(IDOK, [](HWND hwnd, auto&) {
         EndDialog(hwnd, IDOK);
       });

     The handler must be invocable as: handler(HWND, command_message_params&)
  */
  template<typename Handler>
  message_handler& on_command(WORD id, Handler&& handler) {
    return on_message<WM_COMMAND>(handler, command_filter{.id = id});
  }

  /*
     Registers a handler for a specific WM_COMMAND control ID and invokes a handler
     without any parameters when the command is received.

     This overload is useful when the associated action does not need to inspect the message
     parameters and can be triggered directly.

     Example:
       on_command_invoke(IDOK, [] {
         SaveAndExit(TRUE);
       });

     The handler must be invocable as: handler()
  */
  template<typename Handler>
  requires std::invocable<Handler>
  message_handler& on_command_invoke(WORD id, Handler&& handler) {
    return on_command(id, [handler = std::forward<Handler>(handler)](HWND, auto&) mutable {
        std::invoke(handler);
      });
  }

  /*
     Registers a WM_COMMAND handler for a specific control ID, invoking the provided handler
     with HWND as the first argument, followed by any additional user-supplied arguments.

     This overload is ideal for handler that require a window handle along with fixed arguments,
     such as standard Windows API functions.

     Examples:
       on_command_invoke(IDOK, EndDialog, IDOK);
       on_command_invoke(IDC_SHOW_INFO, MessageBoxW, L"Title", L"Info", MB_OK);

     The handler must be invocable as: handler(HWND, Args...)
  */
  template<typename Handler, typename... Args>
  requires std::invocable<Handler, HWND, Args...>
  message_handler& on_command_invoke(WORD id, Handler&& handler, Args&&... args) {
    return on_command(id,
        [handler = std::forward<Handler>(handler),
        ...captured_args = std::forward<Args>(args)](HWND hwnd, auto&) mutable {
      std::invoke(handler, hwnd, captured_args...);
    });
  }

  /*
     Registers a WM_COMMAND handler for a specific control ID, invoking the provided handler
     with fixed arguments (excluding HWND). This is useful when the handler does not require
     a window handle and only needs fixed arguments.

     This is typically used for functions or functors that operate independently of HWND.

     Examples:
       on_command_invoke(IDOK, PostQuitMessage, 0);
       on_command_invoke(IDC_LOG, some_logger_function, "Button clicked");

     The handler must be invocable as: handler(Args...)
     At least one argument must be provided.
  */
  template<typename Handler, typename... Args>
  requires (sizeof...(Args) > 0) && std::invocable<Handler, Args...>
  message_handler& on_command_invoke(WORD id, Handler&& handler, Args&&... args) {
    return on_command(id,
        [handler = std::forward<Handler>(handler),
        ...captured_args = std::forward<Args>(args)](HWND, auto&) mutable {
      std::invoke(handler, captured_args...);
    });
  }

  /*
     Registers a handler for a specific WM_COMMAND message identified by a control ID (`id`)
     and a notification code (`NotifCode`). The handler is invoked with the full `command_params`
     structure, allowing access to both the `id` and the `notif_code`, as well as other metadata.

     This is useful for handling control notifications like `BN_CLICKED` for buttons or
     `EN_CHANGE` for edit controls, in a type-safe way.

     The handler must be invocable as: `handler(HWND, command_params&)`.

     If the handler returns `void`, this wrapper will automatically return 0 to Windows.

     Examples:
       on_command_notify<BN_CLICKED>(IDC_MY_BUTTON, [](HWND hwnd, command_params& params) {
         MessageBox(hwnd, L"Button clicked", L"Info", MB_OK);
       });

     Internally, this function wraps the handler and uses a `command_filter` to ensure the
     correct combination of `id` and `notif_code` are matched before invoking the callback.
  */
  template<WORD NotifCode, typename Handler>
  message_handler& on_command_notify(WORD id, Handler&& handler) {
    return on_message<WM_COMMAND>([handler = std::forward<Handler>(handler)](HWND hwnd, command_params& params) mutable {
      using handler_result_type = std::invoke_result_t<Handler, HWND, command_params&>;

      if constexpr (std::is_same_v<handler_result_type, void>) {
        handler(hwnd, params);
        return 0; // auto-return 0 if handler returns void
      } else {
        return handler(hwnd, params);
      }
    }, command_filter{.id = id, .notif_code = NotifCode});
  }

  /*
     Registers a handler for a WM_COMMAND message with a specific control ID (`id`) and
     notification code (`NotifyCode`), invoking a handler that takes no arguments.

     This overload is useful when the callback does not need to access message parameters,
     and the match is based solely on the ID and notification code.

     The supplied handler must be invocable with zero arguments: `handler()`.

     Example:
       on_command_notify_invoke<BN_CLICKED>(IDC_MY_BUTTON, my_signal);

     Internally, this wraps the zero-argument handler inside a lambda that discards
     the usual `HWND` and `command_params` parameters, providing a concise way to bind
     simple actions to control notifications.
  */
  template<WORD NotifyCode, typename Handler>
  requires std::invocable<Handler>
  message_handler& on_command_notify_invoke(WORD id, Handler&& handler) {
    return on_command_notify<NotifyCode>(id, [handler = std::forward<Handler>(handler)](HWND, auto&) mutable {
        std::invoke(handler);
      });
  }

  /*
     Dispatches a Windows message to a matching registered handler.

     Parameters:
       hwnd   - Handle to the window receiving the message.
       msg    - The message ID (e.g., WM_PAINT, WM_COMMAND).
       wparam - Additional message-specific information.
       lparam - Additional message-specific information.

     Returns:
       An `std::optional<LRESULT>` containing the result of the first handler
       that returns a value, or `std::nullopt` if no handler returns a value.
  */
  std::optional<LRESULT> call_handler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) const {
    auto it = handlers_.find(msg);
    if (it != handlers_.end()) {
      message_params params{wparam, lparam};
      for (const auto& callback : it->second)
        if (auto result = callback(hwnd, params))
          return result;
    }

    return std::nullopt;
  }

private:
  using handler_fn = std::function<std::optional<LRESULT>(HWND, message_params&)>;
  std::unordered_map<UINT, std::vector<handler_fn>> handlers_;
};

}
