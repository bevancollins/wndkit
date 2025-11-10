add_executable(basic_example WIN32
  basic_example.cpp
  app.manifest
)

target_link_libraries(basic_example
  wndkit::widgets
  comctl32
)
