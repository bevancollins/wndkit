// Copyright (c) 2025 Bevan Collins
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include <windows.h>
#include <optional>
#include "wndkit/details/message_traits.hpp"
#include "wndkit/details/notify_traits.hpp"

namespace wndkit {

struct no_filter {
  bool matches([[maybe_unused]] const auto& params) const {
    return true;
  }
};

struct command_filter {
  std::optional<WORD> id;
  std::optional<WORD> notif_code;
  bool matches(const command_params& params) const {
    return
      (!id.has_value() || id.value() == params.id()) &&
      (!notif_code.has_value() || notif_code.value() == params.control_notif_code());
  }
};

struct notify_filter {
  std::optional<UINT> code;
  std::optional<UINT_PTR> id_from;
  bool matches(const notify_params& params) const {
    return
      (!code.has_value() || code.value() == params.nmhdr().code) &&
      (!id_from.has_value() || id_from.value() == params.nmhdr().idFrom);
  }
};

}
