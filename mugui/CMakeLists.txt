# This is for the dynamic C library, not the Go interface.

set(FLTK_DIR "fltk_build/build"
    CACHE FILEPATH "FLTK installation or build directory")

find_package(FLTK 1.4 CONFIG REQUIRED)

add_library(mugui SHARED
  callback.h
  editform.cpp
  functions.cpp functions.h
  iofile.cpp iofile.h
  layout.cpp layout.h
  minion.cpp minion.h
  minion_gui.cpp
  rowtable.cpp rowtable.h
  support_functions.cpp support_functions.h
  textline.cpp
  widget.cpp widget.h
  widgets.cpp widgets.h
  dialogs.cpp
  dialogs.h
)

set_property(TARGET mugui PROPERTY CXX_STANDARD 20)
set_property(TARGET mugui PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(mugui fltk)
