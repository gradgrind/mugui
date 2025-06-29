add_executable(mugui_test1
    main.cpp
)

target_include_directories(mugui_test1
  PRIVATE ../mugui
)

set_property(TARGET mugui_test1 PROPERTY CXX_STANDARD 20)
set_property(TARGET mugui_test1 PROPERTY BUILD_WITH_INSTALL_RPATH ON)
set_property(TARGET mugui_test1 PROPERTY INSTALL_RPATH "$ORIGIN/../mugui")

target_link_libraries(mugui_test1 PRIVATE mugui)
