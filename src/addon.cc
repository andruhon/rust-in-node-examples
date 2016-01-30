#ifndef NATIVE_EXTENSION_GRAB_H
#define NATIVE_EXTENSION_GRAB_H

#include <nan.h>

NAN_METHOD(num_in_num_out);

#endif


extern "C" {
  extern int32_t rs_num_in_num_out(int32_t input);
}

NAN_METHOD(num_in_num_out) {
  int value = info[0]->IsUndefined() ? 0 : Nan::To<int>(info[0]).FromJust();

  int result = rs_num_in_num_out(value);
  info.GetReturnValue().Set(result);
}

using v8::FunctionTemplate;

NAN_MODULE_INIT(InitAll) {;
  Nan::Set(target, Nan::New("num_in_num_out").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(num_in_num_out)).ToLocalChecked());
}

NODE_MODULE(addon, InitAll)
