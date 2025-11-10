#pragma once

#include <windows.h>
#include <commctrl.h>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <wil/resource.h>
#include "hyperlink.hpp"

namespace wndkit::widgets {

class layout {
public:
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

  explicit layout()
    : layout({4, 4}) {
  }

  explicit layout(SIZE spacing_dlu) :
    spacing_dlu_(spacing_dlu) {
  }

  void set_margin(SIZE margin_dlu) {
    margin_dlu_ = margin_dlu;
  }

  auto& add_widget(HWND hwnd, alignment_flag align = alignment_flag::none, std::optional<SIZE> size_dlu = {}) {
    if (!size_dlu.has_value())
      size_dlu = get_standard_control_size_dlu(hwnd);

    assert(size_dlu.has_value());
    items_.push_back(std::make_unique<widget_child_item>(align, hwnd, size_dlu.value()));

    return *this;
  }

  auto& add_layout(std::unique_ptr<layout> layout, alignment_flag align = alignment_flag::none) {
    items_.push_back(std::make_unique<layout_child_item>(align, std::move(layout)));

    return *this;
  }

  void set_font(HWND hwnd, HFONT font) {
    auto hdc = wil::GetDC(hwnd);
    auto old_font = wil::SelectObject(hdc.get(), font);

    TEXTMETRICW tm;
    if (GetTextMetricsW(hdc.get(), &tm))
      set_font_size({tm.tmAveCharWidth, tm.tmHeight});
  }

  void resize(const RECT& area) {
    auto hdwp = BeginDeferWindowPos(0);
    resize(hdwp, area);
    EndDeferWindowPos(hdwp);
  }

  virtual SIZE calc_size() const = 0;

protected:
  virtual void resize(HDWP& hdwp, const RECT& area) = 0;

  void set_font_size(SIZE size) {
    font_size_ = size;

    for (auto& item : items_)
      item->set_font_size(font_size_);
  }

  class child_item {
  public:
    child_item(alignment_flag alignment) :
      alignment_(alignment) {
    }

    auto alignment() const {
      return alignment_;
    }

    virtual SIZE calc_size() = 0;
    virtual void resize(HDWP& hwdp, RECT area) = 0;
    virtual void set_font_size(SIZE size) = 0;

  private:
    alignment_flag alignment_;
  };

  class widget_child_item : public child_item {
  public:
    widget_child_item(alignment_flag alignment, HWND hwnd, SIZE size_dlu) :
      child_item(alignment),
      hwnd_(hwnd),
      size_dlu_(size_dlu) {
    }

    auto hwnd() {
      return hwnd_;
    }

    SIZE calc_size() override {
      return size_dlu_;
    }

    void resize(HDWP& hdwp, RECT area) override {
      hdwp = DeferWindowPos(hdwp, hwnd_, 0, area.left, area.top, area.right - area.left, area.bottom - area.top, SWP_NOZORDER);
    }

    void set_font_size(SIZE) override {}

  private:
    HWND hwnd_;
    SIZE size_dlu_;
  };

  class layout_child_item : public child_item {
  public:
    layout_child_item(alignment_flag alignment, std::unique_ptr<layout> layout) :
      child_item(alignment),
      layout_(std::move(layout)) {
    }

    SIZE calc_size() override {
      return layout_->calc_size();
    }

    void resize(HDWP& hdwp, RECT area) override {
      layout_->resize(hdwp, area);
    }

    void set_font_size(SIZE size) override {
      layout_->set_font_size(size);
    }

  private:
    std::unique_ptr<layout> layout_;
  };

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
    } else if (class_name == hyperlink::class_name()) {
      return {{50, 8}};
    }

    return {};
  }

  // Compute base unit size (DLU)
  SIZE to_pixels(SIZE dlu) {
    return {
      MulDiv(dlu.cx, font_size_.cx, 4),
      MulDiv(dlu.cy, font_size_.cy, 8)
    };
  }

  SIZE font_size_{};
  std::vector<std::unique_ptr<child_item>> items_;
  SIZE margin_dlu_{};
  SIZE spacing_dlu_{};
};

}
