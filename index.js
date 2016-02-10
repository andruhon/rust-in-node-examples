os = require('os');
addon = require('./build/Release/addon');

console.log(os.EOL+"int_in_int_out:");
console.log(addon.int_in_int_out(2));

console.log(os.EOL+"string_in_string_out:");
console.log(addon.string_in_string_out("String from JS UTF-8 with a lot of non latin symbols АБВГДеёжйклмнопрстуфхцчшщЫЭЮЯ"));


// TODO: this one sometimes crashing, to investigate
// console.log(os.EOL+"bin_string_in_string_out:"); //sometimes crashing
// var data = "Another string from JS UTF-8 with a lot of non latin symbols АБВГДеёжйклмнопрстуфхцчшщЫЭЮЯ";
// length = Buffer.byteLength(data); //UTF8 length
// var buf = new Buffer(length);
// buf.write(data, 0, 'UTF8');
// console.log(addon.bin_string_in_string_out(buf, length));

console.log(os.EOL+"numeric_array_in_numeric_array_out:");
console.log(addon.numeric_array_in_numeric_array_out([1,2,3,4]));

console.log(os.EOL+"struct_out:");
console.log(addon.struct_out_as_object());

console.log(os.EOL+"object_as_struct_in_bool_out:");
console.log(addon.object_as_struct_in_bool_out({
  int_setting: 42,
  float_setting: 3.14,
  bool_setting: true
}));

//// This one needs some kind of timeout to prevent node from killing it
//// Left here for reference
// console.log(os.EOL+"num_c_func_with_c_callback:");
// addon.num_c_func_with_c_callback(1);
// console.log("JS waiting for response...");
// setTimeout(function(){
//   console.log("JS timeout...");
// },3000);

console.log(os.EOL+"set_initial_state:");
addon.set_initial_state(2);

console.log(os.EOL+"num_c_func_with_c_callback:");
addon.slow_func_in_c_thread(12, function(){
  console.log("slow func callback", arguments);
  addon.slow_func_in_c_thread(12, console.log);
  addon.slow_func_in_c_thread(14, console.log);
  addon.slow_func_in_c_thread(18, console.log);
});
