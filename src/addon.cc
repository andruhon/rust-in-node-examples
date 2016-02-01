//Header
//This could go into separate header file defining interface:
#ifndef NATIVE_EXTENSION_GRAB_H
#define NATIVE_EXTENSION_GRAB_H

#include <nan.h>
#include <string>
#include <iostream>
#include <node.h>

/*
// Public interfaces of functions could be defined here
NAN_METHOD(num_in_num_out);
//...
*/

#endif

/* extern interface for Rust functions */
extern "C" {
  int32_t rs_num_in_num_out(int32_t input);
  int32_t rs_string_in_string_out(char* input, char* output);
}
//Header end


using namespace std;
using namespace v8;

NAN_METHOD(num_in_num_out) {
  int value = info[0]->IsUndefined() ? 0 : Nan::To<int>(info[0]).FromJust();

  int result = rs_num_in_num_out(value);
  info.GetReturnValue().Set(result);
}

NAN_METHOD(string_in_string_out) {
  Nan::HandleScope scope;
  String::Utf8Value cmd(info[0]);
  string s = string(*cmd);
  char *cstr = new char[s.length() + 1];
  strcpy(cstr, s.c_str());

  /* some buffer, output string should not exceed this buffer */
  char* from_rust = (char*) malloc (16000);

  /* get string from rust */
  int32_t len = rs_string_in_string_out(cstr, from_rust);
  info.GetReturnValue().Set(Nan::New<String>(from_rust, len).ToLocalChecked());

  free(from_rust);
}

NAN_METHOD(bin_string_in_string_out) {
  Local<Object> bufferObj = info[0]->ToObject();
  unsigned int len = info[1]->Uint32Value();
  char* msg = node::Buffer::Data(bufferObj);

  char *cstr = new char[len + 1];
  strcpy(cstr, msg);

  /* some buffer, output string should not exceed this buffer */
  char* from_rust = (char*) malloc (16000);
  int32_t from_rust_len = rs_string_in_string_out(cstr, from_rust);

  info.GetReturnValue().Set(Nan::New<String>(from_rust, from_rust_len).ToLocalChecked());
  free(from_rust);
}

/* create V8 functions available in NodeJS */
using v8::FunctionTemplate;

NAN_MODULE_INIT(InitAll) {;
  Nan::Set(target, Nan::New("num_in_num_out").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(num_in_num_out)).ToLocalChecked());
  Nan::Set(target, Nan::New("string_in_string_out").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(string_in_string_out)).ToLocalChecked());
  Nan::Set(target, Nan::New("bin_string_in_string_out").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(bin_string_in_string_out)).ToLocalChecked());
}


NODE_MODULE(addon, InitAll)
