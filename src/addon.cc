//Header
//This could go into separate header file defining interface:
#ifndef NATIVE_EXTENSION_GRAB_H
#define NATIVE_EXTENSION_GRAB_H

#include <nan.h>
#include <string>
#include <iostream>
#include <node.h>
#include <stdio.h>

/*
// Public interfaces of functions could be defined here
NAN_METHOD(int_in_int_out);
//...
*/

#endif

struct SomeStruct {
    int some_item;
    int another_item;
    int test;
    float float_item;
};

/* extern interface for Rust functions */
extern "C" {
  int32_t rs_int_in_int_out(int32_t input);
  int32_t rs_string_in_string_out(char* input, char* output);
  int32_t rs_numeric_array_in_numeric_array_out(int32_t src[4], int32_t dst[4], int32_t size);
  SomeStruct rs_struct_out();
}

/**
 * It works fine for now, but I shoud do something about that in future
 */
const int ADDON_BUFFER_SIZE = 16383;

//Header end


using namespace std;
using namespace v8;

NAN_METHOD(int_in_int_out) {
  int value = info[0]->IsUndefined() ? 0 : Nan::To<int>(info[0]).FromJust();

  int result = rs_int_in_int_out(value);
  info.GetReturnValue().Set(result);
}

NAN_METHOD(string_in_string_out) {
  Nan::HandleScope scope;
  String::Utf8Value cmd(info[0]);
  string s = string(*cmd);
  char *cstr = new char[s.length() + 1];
  strcpy(cstr, s.c_str());

  /* some buffer, output string should not exceed this buffer */
  //We could, probably do this malloc in rust: let ptr = malloc(mem::size_of::<T>() as size_t) as *mut T;
  char* from_rust = (char*) malloc (ADDON_BUFFER_SIZE);

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
  char* from_rust = (char*) malloc (ADDON_BUFFER_SIZE);
  int32_t from_rust_len = rs_string_in_string_out(cstr, from_rust);

  info.GetReturnValue().Set(Nan::New<String>(from_rust, from_rust_len).ToLocalChecked());
  free(from_rust);
}

NAN_METHOD(numeric_array_in_numeric_array_out) {
  Nan::HandleScope scope;
  Handle<Value> val;
  Local<Array> arr = Nan::New<Array>();

  if (info[0]->IsArray()) {
    Handle<Array> jsArray = Handle<Array>::Cast(info[0]);
    unsigned int in_len = jsArray->Length();
    int32_t* input = (int32_t*) malloc (in_len+1);
    int32_t* output = (int32_t*) malloc (ADDON_BUFFER_SIZE); //damnd buffer.
    for (unsigned int i = 0; i < in_len; i++) {
      val = jsArray->Get(i);
      input[i] = Nan::To<int>(val).FromJust();
    }

    unsigned int from_rust_size = rs_numeric_array_in_numeric_array_out(input, output, in_len);

    for (unsigned int i = 0; i < from_rust_size; i++) {
      Nan::Set(arr, i, Nan::New(output[i]));
    }
    free(input);
    free(output);
  }

  info.GetReturnValue().Set(arr);
}

NAN_METHOD(struct_out_as_object) {
  SomeStruct outstru = rs_struct_out();

  v8::Local<v8::Object> obj = Nan::New<v8::Object>();
  Nan::Set(obj, Nan::New("some_item").ToLocalChecked(), Nan::New(outstru.some_item));
  Nan::Set(obj, Nan::New("another_item").ToLocalChecked(), Nan::New(outstru.another_item));
  Nan::Set(obj, Nan::New("test").ToLocalChecked(), Nan::New(outstru.test));
  Nan::Set(obj, Nan::New("float_item").ToLocalChecked(), Nan::New(outstru.float_item));
  info.GetReturnValue().Set(obj);
}

/* create V8 functions available in NodeJS */
using v8::FunctionTemplate;

NAN_MODULE_INIT(InitAll) {;
  Nan::Set(target, Nan::New("int_in_int_out").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(int_in_int_out)).ToLocalChecked());
  Nan::Set(target, Nan::New("string_in_string_out").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(string_in_string_out)).ToLocalChecked());
  Nan::Set(target, Nan::New("bin_string_in_string_out").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(bin_string_in_string_out)).ToLocalChecked());
  Nan::Set(target, Nan::New("numeric_array_in_numeric_array_out").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(numeric_array_in_numeric_array_out)).ToLocalChecked());
  Nan::Set(target, Nan::New("struct_out_as_object").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(struct_out_as_object)).ToLocalChecked());
}


NODE_MODULE(addon, InitAll)
