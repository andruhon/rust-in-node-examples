os = require('os');
addon = require('./build/Release/addon');

console.log(os.EOL+"num_in_num_out:");
console.log(addon.num_in_num_out(2));

console.log(os.EOL+"string_in_string_out:");
console.log(addon.string_in_string_out("String from JS UTF-8 with a lot of non latin symbols АБВГДеёжйклмнопрстуфхцчшщЫЭЮЯ"));


console.log(os.EOL+"bin_string_in_string_out:"); //sometimes crashing
var data = "Another string from JS UTF-8 with a lot of non latin symbols АБВГДеёжйклмнопрстуфхцчшщЫЭЮЯ";
length = Buffer.byteLength(data); //UTF8 length
var buf = new Buffer(length);
buf.write(data, 0, 'UTF8');
console.log(addon.bin_string_in_string_out(buf, length));
