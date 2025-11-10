#pragma once

#include "box_layout.hpp"

namespace wndkit::widgets {

class hbox_layout : public box_layout {
public:
  explicit hbox_layout() :
    box_layout(box_layout::orientation::horizontal) {
  }
};

}
