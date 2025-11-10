#pragma once

#include "layout.hpp"

namespace wndkit::widgets {

class box_layout : public layout {
public:
  enum class orientation {
    horizontal,
    vertical
  };

  explicit box_layout(orientation orientation)
    : orientation_(orientation) {
  }

  // returns the space that the layout will take up (in dialog units)
  virtual SIZE calc_size() const override {
    SIZE size{margin_dlu_.cx * 2, margin_dlu_.cy * 2};
    if (orientation_ == orientation::horizontal)
      size.cx += static_cast<LONG>(items_.size() - 1) * spacing_dlu_.cx;
    else
      size.cy += static_cast<LONG>(items_.size() - 1) * spacing_dlu_.cy;

    SIZE total_items_size{};
    for (const auto& item : items_) {
      auto item_size = item->calc_size();
      if (orientation_ == orientation::horizontal) {
        total_items_size.cx += item_size.cx;
        if (item_size.cy > total_items_size.cy)
          total_items_size.cy = item_size.cy;
      } else {
        total_items_size.cy += item_size.cy;
        if (item_size.cx > total_items_size.cx)
          total_items_size.cx = item_size.cx;
      }
    }

    size.cx += total_items_size.cx;
    size.cy += total_items_size.cy;

    return size;
  }

  virtual void resize(const RECT& area) override {
    auto margin = to_pixels(margin_dlu_);
    auto spacing = to_pixels(spacing_dlu_);

    POINT item_pos{area.left + margin.cx, area.top + margin.cy};

    for (const auto& item : items_) {
      auto item_size{to_pixels(item->calc_size())};

      auto aligned_pos{item_pos};
      if (orientation_ == orientation::horizontal) {
        if ((item->alignment() & alignment_flag::align_vcenter) == alignment_flag::align_vcenter)
          aligned_pos.y = item_pos.y + (area.bottom - area.top - margin.cy - item_size.cy) / 2;
        else if ((item->alignment() & alignment_flag::align_bottom) == alignment_flag::align_bottom)
          aligned_pos.y = area.bottom - margin.cy - item_size.cy;

        item_pos.x += item_size.cx + spacing.cx;
      } else {
        if ((item->alignment() & alignment_flag::align_hcenter) == alignment_flag::align_hcenter)
          aligned_pos.x = item_pos.x + (area.right - area.left - margin.cx - item_size.cx) / 2;
        else if ((item->alignment() & alignment_flag::align_right) == alignment_flag::align_right)
          aligned_pos.x = area.right - margin.cx - item_size.cx;

        item_pos.y += item_size.cy + spacing.cy;
      }

      RECT item_area{
        aligned_pos.x,
        aligned_pos.y,
        aligned_pos.x + item_size.cx,
        aligned_pos.y + item_size.cy
      };

      item->resize(item_area);
    }
  }

protected:
  orientation orientation_;
};

}
