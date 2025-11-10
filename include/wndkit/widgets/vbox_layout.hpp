#pragma once

#include "box_layout.hpp"

namespace wndkit::widgets {

class vbox_layout : public box_layout {
public:
  explicit vbox_layout() :
    box_layout(box_layout::orientation::vertical) {
  }
};

}
