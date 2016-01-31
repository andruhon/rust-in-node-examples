#ifndef NATIVE_EXTENSION_GRAB_H
#define NATIVE_EXTENSION_GRAB_H

#include <nan.h>
#include <string>
#include <iostream>
#include <node.h>

NAN_METHOD(num_in_num_out);

#endif

using namespace std;
using namespace v8;



extern "C" {
  int32_t rs_num_in_num_out(int32_t input);
  char* rs_string_in_string_out(char* input);
}

NAN_METHOD(num_in_num_out) {
  int value = info[0]->IsUndefined() ? 0 : Nan::To<int>(info[0]).FromJust();

  int result = rs_num_in_num_out(value);
  info.GetReturnValue().Set(result);
}

NAN_METHOD(string_in_string_out) {
  /* see http://www.puritys.me/docs-blog/article-286-How-to-pass-the-paramater-of-Node.js-or-io.js-into-native-C/C++-function..html */
  /* Get String and convert to char */

  /*
  //This one is for binary string
  Local<Object> bufferObj = info[0]->ToObject();
  unsigned int len = info[1]->Uint32Value(); //length
  char* msg = node::Buffer::Data(bufferObj);
  */

  /* this one is for ordinary String */
  Nan::HandleScope scope;
    String::Utf8Value cmd(info[0]);
    string s = string(*cmd);

  char *cstr = new char[s.length() + 1];
  strcpy(cstr, s.c_str());


  char* from_rust = rs_string_in_string_out(cstr);
  info.GetReturnValue().Set(Nan::New(from_rust).ToLocalChecked());
}

using v8::FunctionTemplate;

NAN_MODULE_INIT(InitAll) {;
  Nan::Set(target, Nan::New("num_in_num_out").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(num_in_num_out)).ToLocalChecked());
  Nan::Set(target, Nan::New("string_in_string_out").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(string_in_string_out)).ToLocalChecked());
}

NODE_MODULE(addon, InitAll)
