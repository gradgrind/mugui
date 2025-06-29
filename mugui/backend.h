#ifndef BACKEND_H
#define BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

void SetCallbackFunction(const char* (*backend_pointer)(const char*) );

void Init(const char* data0);

#ifdef __cplusplus
}
#endif
#endif // BACKEND_H
