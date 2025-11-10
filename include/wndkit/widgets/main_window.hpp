#pragma once

#include "top_level_window.hpp"

namespace wndkit::widgets {

class main_window : public top_level_window {
public:
  main_window()
    : top_level_window() {
    message_handler_
      .on_message_invoke<WM_DESTROY>(PostQuitMessage, 0);
  }
};

}
