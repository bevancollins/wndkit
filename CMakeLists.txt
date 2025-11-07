cmake_minimum_required(VERSION 3.15)
project(wndkit)

option(WNDKIT_BUILD_EXAMPLES "Build wndkit example programs" ON)

add_library(wndkit INTERFACE
  include/wndkit/dispatcher.hpp
  include/wndkit/message_filters.hpp
  include/wndkit/message_handler.hpp
  include/wndkit/message_params.hpp
  include/wndkit/details/message_traits.hpp
  include/wndkit/details/notify_traits.hpp
)
add_library(wndkit::wndkit ALIAS wndkit)

target_include_directories(wndkit INTERFACE
  include
)

target_compile_features(wndkit INTERFACE cxx_std_20)
set_target_properties(wndkit PROPERTIES INTERFACE_HEADER_ONLY ON)

find_package(wil CONFIG QUIET)
find_package(unofficial-webview2 CONFIG QUIET)
if (TARGET WIL::WIL AND TARGET unofficial::webview2::webview2)
  message(STATUS "Building wndkit::widgets")

  add_library(wndkit_widgets INTERFACE
    include/wndkit/widgets/hyperlink.hpp
    include/wndkit/widgets/layout.hpp
    include/wndkit/widgets/main_window.hpp
    include/wndkit/widgets/top_level_window.hpp
    include/wndkit/widgets/web_view.hpp
  )
  add_library(wndkit::widgets ALIAS wndkit_widgets)

  target_link_libraries(wndkit_widgets
    INTERFACE
      wndkit::wndkit
      WIL::WIL
      unofficial::webview2::webview2
  )
else()
  message(STATUS "Skipping wndkit::widgets (wil/webview2 not found)")
endif()

if(WNDKIT_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
