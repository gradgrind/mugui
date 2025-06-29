#include "connector.h"
#include "backend.h"

// _cgo_export.h is auto-generated and has Go //export funcs
#include "_cgo_export.h"

void init(cchar_t* data0) {
    SetCallbackFunction(goCallback);
    Init(data0);
}
  