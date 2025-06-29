# mugui

The basic idea is to control an FLTK-based GUI via a simple interface using MINION messages.

[MINION](https://github.com/gradgrind/minion) is a simple structured notation with some resemblance to JSON. Its primary design aims were to be easier to type manually than JSON and to avoid some of JSON's "woolly" areas (number type and precisiom, unicode escapes as UTF-16). It only supports the data types string, map/object and list/array. Quotation of strings is often unnecessary, especially for simple object keys. Comments and multiline strings are supported in a fairly clear way.

`mugui`_ is not intended to be a comprehensive wrapper around FLTK, but to provide a convenient text-based interface to some of the widgets. Also some custom, higher-level, widgets are defined for convenience.
As far as layouts are concerned, the `Fl_Grid` widget is strongly preferred. Single-row and single-column wrappers are provided to cover common usage patterns.

The wrapper is written in C++ and can be compiled to a small dynamic library with a very simple interface, covered in the header file `backend.h`. The function `SetCallbackFunction` is used to register a function which will be called when the GUI generates a callback – passing a MINION message and returning  a MINION result. The GUI is initialized by calling the function `Init` with an initial MINION description of the GUI. The GUI is generated and the event loop is started. Subsequently, the GUI takes over as "front-end" and all further communication is done via callbacks. Thus, `Init` only returns when the front-end (GUI) is closed, i.e. at program exit.

There is also an interface to Go which consists, in essence, of only a single function, `MinionGui`. This passes an initial MINION message to the wrapper describing the GUI widgets and starting the event loop. A second argument to `MinionGui` registers a Go function to receive callbacks. This callback function receives a single argument, a MINION string, and returns a MINION string as result to the front-end (GUI). `MinionGui` returns when the GUI is closed, i.e. at program exit.

Callbacks happen within the main thread, so the front-end is blocked while they are being processed. Thus they should return quickly.

## Longer-running callbacks

To perform more extensive back-end processing, a separate thread should be used. The front-end must be made aware that a longer-running back-end process is under way, because it is designed around a single callback, multiple concurrent callbacks are not supported. To handle this, a special flag is returned by a callback when it returns without having completed. The front-end then sends further "follow-up" callbacks to poll the progress of the back-end process. If the back-end process exceeds a certain time-limit, the front-end displays a "dialog" pop-up which can show progress reports (if these are sent by the back-end) and allow the cancellation of the back-end process, if desired – and if supported by the back-end.

*TODO*: Include a helper function to manage such longer-running callbacks?

## Details

### minion_gui.go

`MinionGui` saves the `callback` function in the variable `do_callback`, where it can be used in actual callbacks by the function `goCallback`, which is exported via `cgo` to the C++ front-end. The string argument `initdata` is MINION data describing the initial GUI configuration. This is passed to the C++ function `init` in `minion_gui.cpp` after creating a new C-string from it. The same Go static variable, `resultptr`, is used for this C-string as for the C-string returned in callbacks by `goCallback`. That means the C-string from `initdata` is available until the first callback. On return from `init` the C-string is also freed. At this point the front-end has been closed, so the program is exiting. Nevertheless, for the sake of tidiness, the C-string is freed here, whether it be the last callback result or that from `initdata` (if there was no callback).

`goCallback` receives and returns C-strings. To connect to the Go callback function passed to `MinionGui` these must be converted to or from Go strings. The C-string passed to `goCallback` is managed by the C++ side (function `backend` in `minion_gui.cpp`) and is valid for the duration of the `goCallback` call. The C-string returned to `backend` is created by `goCallback` and remains valid until the next call to `backend`, so if the C++ side needs it longer it must copy it. As only one callback is permitted at any time, this simple scheme should be adequate.
