//Header
//This could go into separate header file defining interface:
#ifndef NATIVE_EXTENSION_GRAB_H
#define NATIVE_EXTENSION_GRAB_H

#include <nan.h>
#include <string>
#include <iostream>
#include <node.h>
#include <stdio.h>


using namespace std;
using namespace v8;

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

struct OtherStruct {
  int int_setting;
  float float_setting;
  bool bool_setting;
};

/* extern interface for Rust functions */
extern "C" {
  int32_t rs_int_in_int_out(int32_t input);
  int32_t rs_string_in_string_out(char* input, char* output);
  int32_t rs_numeric_array_in_numeric_array_out(int32_t src[4], int32_t dst[4], int32_t size);
  SomeStruct rs_struct_out();
  bool rs_object_as_struct_in_bool_out(OtherStruct object_as_struct_in_bool_out);
  // void rs_num_c_func_with_c_callback(int32_t input, void (*callback)(int32_t));
  void rs_set_initial_state(int32_t input);
  int32_t rs_slow_func(int32_t input);
}

/**
 * It works fine for now, but I shoud do something about that in future
 */
const int ADDON_BUFFER_SIZE = 16383;

//Header end



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

  Local<v8::Object> obj = Nan::New<Object>();
  Nan::Set(obj, Nan::New("some_item").ToLocalChecked(), Nan::New(outstru.some_item));
  Nan::Set(obj, Nan::New("another_item").ToLocalChecked(), Nan::New(outstru.another_item));
  Nan::Set(obj, Nan::New("test").ToLocalChecked(), Nan::New(outstru.test));
  Nan::Set(obj, Nan::New("float_item").ToLocalChecked(), Nan::New(outstru.float_item));
  info.GetReturnValue().Set(obj);
}

NAN_METHOD(object_as_struct_in_bool_out) {
  if (info[0]->IsObject()) {
    /* get data from the js object */
    Local<Object> obj = Nan::To<Object>(info[0]).ToLocalChecked();
    int int_setting = Nan::To<int>(Nan::Get(obj, Nan::New("int_setting").ToLocalChecked()).ToLocalChecked()).FromJust();
    double float_setting = Nan::To<double>(Nan::Get(obj, Nan::New("float_setting").ToLocalChecked()).ToLocalChecked()).FromJust();
    bool bool_setting = Nan::To<bool>(Nan::Get(obj, Nan::New("bool_setting").ToLocalChecked()).ToLocalChecked()).FromJust();

    /* define struct with data */
    OtherStruct instru;
    instru.int_setting = int_setting;
    instru.float_setting = float_setting;
    instru.bool_setting = bool_setting;

    /* send struct to the rust function */
    bool result = rs_object_as_struct_in_bool_out(instru);
    info.GetReturnValue().Set(result);
  }
  info.GetReturnValue().Set(false);
}

// extern "C" void mycallback(int32_t val) {
//   // Left here for the reference
//   cout << "num_c_func_with_c_callback in C++: ";
//   cout << val;
//   cout << "\n";
// }
// NAN_METHOD(num_c_func_with_c_callback) {
//     int value = info[0]->IsUndefined() ? 0 : Nan::To<int>(info[0]).FromJust();
//
//     rs_num_c_func_with_c_callback(value, &mycallback);
// }

NAN_METHOD(set_initial_state) {
  int value = info[0]->IsUndefined() ? 0 : Nan::To<int>(info[0]).FromJust();

  rs_set_initial_state(value);
}


using v8::Function;
using v8::Local;
using v8::Number;
using v8::Value;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::New;
using Nan::Null;
using Nan::To;

/* a copypaste from NAN async example */
class MyWorker : public AsyncWorker {
 public:
  MyWorker(Callback *callback, int value)
    : AsyncWorker(callback), value(value), result(0) {}
  ~MyWorker() {}

  // Executed inside the worker-thread.
  // It is not safe to access V8, or V8 data structures
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute () {
    result = rs_slow_func(value);
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback () {
    Nan::HandleScope scope;

    Local<Value> argv[] = {
        Null()
      , New<Number>(result)
    };

    callback->Call(2, argv);
  }

 private:
  int value;
  int result;
};

NAN_METHOD(slow_func_in_c_thread) {
    int value = info[0]->IsUndefined() ? 0 : Nan::To<int>(info[0]).FromJust();
    Nan::Callback * callback = new Nan::Callback(info[1].As<v8::Function>());
    int points = 1;
    AsyncQueueWorker(new MyWorker(callback, value));
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
  Nan::Set(target, Nan::New("object_as_struct_in_bool_out").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(object_as_struct_in_bool_out)).ToLocalChecked());
  // Nan::Set(target, Nan::New("num_c_func_with_c_callback").ToLocalChecked(),
  //   Nan::GetFunction(Nan::New<FunctionTemplate>(num_c_func_with_c_callback)).ToLocalChecked());
  Nan::Set(target, Nan::New("set_initial_state").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(set_initial_state)).ToLocalChecked());
  Nan::Set(target, Nan::New("slow_func_in_c_thread").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(slow_func_in_c_thread)).ToLocalChecked());
}


NODE_MODULE(addon, InitAll)
