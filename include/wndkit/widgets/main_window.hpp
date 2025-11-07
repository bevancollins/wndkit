#pragma once

#include "top_level_window.hpp"

namespace wndkit::widgets {

class main_window : public top_level_window {
public:
  main_window(box_layout::orientation orientation = box_layout::orientation::vertical)
    : top_level_window(orientation) {
    message_handler_
      .on_message_invoke<WM_DESTROY>(PostQuitMessage, 0);
  }
};

}
