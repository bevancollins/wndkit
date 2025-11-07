#pragma once

#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include <wil/resource.h>
#include <wndkit/message_handler.hpp>

namespace wndkit {

class box_layout {
public:
  enum class orientation {
    horizontal,
    vertical
  };

  enum class alignment_flag : uint32_t {
    none          = 0x0,
    align_left    = 0x1,
    align_right   = 0x2,
    align_hcenter = 0x4,
    align_top     = 0x8,
    align_bottom  = 0x10,
    align_vcenter = 0x20,
    align_center  = align_hcenter | align_vcenter
  };

  friend constexpr alignment_flag operator|(alignment_flag lhs, alignment_flag rhs) noexcept {
    using utype = std::underlying_type_t<alignment_flag>;
    return static_cast<alignment_flag>(
      static_cast<utype>(lhs) | static_cast<utype>(rhs)
    );
  }

  friend constexpr alignment_flag operator&(alignment_flag lhs, alignment_flag rhs) noexcept {
    using utype = std::underlying_type_t<alignment_flag>;
    return static_cast<alignment_flag>(
      static_cast<utype>(lhs) & static_cast<utype>(rhs)
    );
  }

  explicit box_layout(wndkit::message_handler& message_handler, orientation orientation)
    : box_layout(message_handler, orientation, {7, 7}) {
  }

  explicit box_layout(wndkit::message_handler& message_handler, orientation orientation, SIZE margin_dlu) :
    orientation_(orientation),
    margin_dlu_(margin_dlu) {

    message_handler
      .on_message<WM_SETFONT>([this](HWND hwnd, const auto& params) {
        on_setfont(hwnd, params);
      })
    ;
  }

  auto& add_widget(HWND hwnd, bool related = true, alignment_flag align = alignment_flag::none, std::optional<SIZE> size_dlu = {}) {
    if (!size_dlu.has_value())
      size_dlu = get_standard_control_size_dlu(hwnd);

    SIZE spacing_dlu;
    if (related)
      spacing_dlu = {4, 4};
    else
      spacing_dlu = {7, 7};

    widgets_.emplace_back(
        hwnd,
        align,
        size_dlu,
        spacing_dlu);

    return *this;
  }

  void resize(const SIZE& size) {
    SIZE margin = to_pixels(margin_dlu_);

    const auto available_width  = size.cx - 2 * margin.cx;
    const auto available_height = size.cy - 2 * margin.cy;

    auto x = margin.cx;
    auto y = margin.cy;

    for (const auto& widget : widgets_) {
      auto size = to_pixels(widget.size_dlu.value());
      auto spacing = to_pixels(widget.spacing_dlu);

      auto pos_x = x;
      auto pos_y = y;

      if (orientation_ == orientation::horizontal) {
        // Vertical alignment
        if ((widget.alignment & alignment_flag::align_vcenter) == alignment_flag::align_vcenter)
          pos_y = margin.cy + (available_height - size.cy) / 2;
        else if ((widget.alignment & alignment_flag::align_bottom) == alignment_flag::align_bottom)
          pos_y = size.cy - margin.cy - size.cy;

        x += size.cx + spacing.cx;
      } else {
        // Horizontal alignment
        if ((widget.alignment & alignment_flag::align_hcenter) == alignment_flag::align_hcenter)
          pos_x = margin.cx + (available_width - size.cx) / 2;
        else if ((widget.alignment & alignment_flag::align_right) == alignment_flag::align_right)
          pos_x = size.cx - margin.cx - size.cx;

        y += size.cy + spacing.cy;
      }

      MoveWindow(widget.hwnd, pos_x, pos_y, size.cx, size.cy, TRUE);
    }
  }

private:
  struct layout_widget {
    HWND hwnd{};
    alignment_flag alignment{alignment_flag::none};
    std::optional<SIZE> size_dlu{};
    SIZE spacing_dlu{};
  };

  void on_setfont(HWND hwnd, const wndkit::setfont_params& params) {
    auto hdc = wil::GetDC(hwnd);
    auto old_font = wil::SelectObject(hdc.get(), params.hfont());

    GetTextMetricsW(hdc.get(), &tm_);
  }

  static std::optional<SIZE> get_standard_control_size_dlu(HWND hwnd) {
    std::wstring class_name(64, L'\0');
    class_name.resize(GetClassNameW(hwnd, class_name.data(), class_name.size()));

    auto style = static_cast<DWORD>(GetWindowLongPtr(hwnd, GWL_STYLE));

    if (class_name == WC_BUTTONW) {
      switch (style & 0xF) { // lower 4 bits are the BS_* type
        case BS_CHECKBOX:
        case BS_AUTOCHECKBOX:
        case BS_3STATE:
        case BS_AUTO3STATE:
        case BS_RADIOBUTTON:
        case BS_AUTORADIOBUTTON:
          return {{10, 10}};

        case BS_GROUPBOX:
          return {{50, 28}};

        default:
          return {{50, 14}};
      }
    } else if (class_name == WC_EDITW) {
      return {{50, 14}};
    } else if (class_name == WC_STATICW) {
      return {{50, 8}};
    } else if (class_name == WC_LISTBOXW) {
      return {{50, 50}};
    } else if (class_name == WC_SCROLLBARW) {
      return {{16, 70}};
    }

    return {};
  }

  // Compute base unit size (DLU)
  SIZE to_pixels(SIZE dlu) {
    return {
      MulDiv(dlu.cx, tm_.tmAveCharWidth, 4),
      MulDiv(dlu.cy, tm_.tmHeight, 8)
    };
  }

  orientation orientation_;
  TEXTMETRICW tm_{};
  std::vector<layout_widget> widgets_;
  SIZE margin_dlu_{};
};

}
