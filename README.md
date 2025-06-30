# mugui
Text-based GUI interface using FLTK

`mugui` is basically a wrapper around the FLTK library allowing communication using text messages in MINION, which is somewhat like a simplified JSON. `mugui` compiles to a dynamic library with a very simple C API. All commands to the GUI and callbacks from it are via MINION strings.

Currently the build scripts (using `cmake`) have only been designed for and tested on Linux-amd64, but it should be possible to adapt them for other operating systems (FLTK should work on MacOS and Windows, too).

There is also a simple Go interface to the library, which allows static linking of FLTK (but not necessarily its dependencies).

The build script for FLTK is written in Go. That means that Go must be installed on the machine used for building. `go_generate` in the `mugui` directory should be enough to compile the fltk library. The script, `fltk-build.go` is a slightly modified version of the one in the package [go-fltk](https://github.com/pwiecz/go-fltk) (thank you!), so it _might_ work on Windows and MacOS. I have changed one or two options and specified POSITION_INDEPENDENT_CODE, so that it is possible to build a dynamic library for the C binding.

A small test program which links to the library is in directory `test1`.

An older, pure C++ version is available in the [minion_gui](https://github.com/gradgrind/minion_gui) repository, so if the Go build-dependency is undesired, the build scripts could be adapted from that.

## Requirements

For building `mugui` you need Go, `cmake` and a C++20 compiler, such as GCC or Clang on Linux, MinGW on Windows or XCode on MacOS. On Ubuntu (24.04) this should be covered by `sudo apt install build-essential cmake`. To compile FLTK, additional development packages may well be necessary. [Go](https://go.dev) should probably be installed from the website, as a distribution's own packages may be too old.

For running programs built using `mugui` you will need some system libs which are normally available on operating systems with a graphical user interfaces:

- (Windows: no external dependencies?)
- (MacOS: no external dependencies?)
- Linux (and other Unix systems): on a fresh Linux Mint (22.1) installation no additional libraries were required.

*Tip*: The [rust-fltk project](https://github.com/fltk-rs/fltk-rs) may be helpful in determining dependencies, both for building and for running. Note, however, that the FLTK configuration will affect this.

## Building the C library and test application

On Linux, once the FLTK libraries have been built, it should be enough to run

`sh mugui_build.sh`

in the root directory.

Then, to run the test program:

`(cd build/release/test1 && ./mugui_test1)`

## Using the Go module

At present, this is not set up for direct usage as a "require" in a `go.mod`. It seems more convenient at present to download the repository and use the `mugui` subdirectory as a local package (e.g. using a symlink).
