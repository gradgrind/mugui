cmake_minimum_required(VERSION 3.16)

project(mugui
    VERSION 1.0.0
    LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

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

add_subdirectory(test1)
