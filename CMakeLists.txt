cmake_minimum_required(VERSION 3.15)
project(wndkit)

add_library(wndkit INTERFACE
  include/wndkit/dispatcher.hpp
  include/wndkit/subclass_dispatcher.hpp
  include/wndkit/commctrl_subclass_dispatcher.hpp
  include/wndkit/message_filters.hpp
  include/wndkit/message_handler.hpp
  include/wndkit/message_params.hpp
  include/wndkit/details/message_traits.hpp
  include/wndkit/details/notify_traits.hpp
)

target_include_directories(wndkit INTERFACE
  include
)

target_compile_features(wndkit INTERFACE cxx_std_20)
set_target_properties(wndkit PROPERTIES INTERFACE_HEADER_ONLY ON)

add_subdirectory(examples)
