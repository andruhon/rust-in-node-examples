Rust in node examples
=====================

Experimenting with adding Rust extension into NodeJS via C++ addon

Work still in progress

Windows only for this moment.
However it is easy to amend binding.gyp and package.json in order to make it work on other OSes.

List of available functions:

* int to int function
* string to string function
* numeric array to numeric array function
* void to struct as an object
* struct as object to bool

Issues
------

I'm using some non cross platform unit sizes, probably 32 bit specific. Will test and fix it later.


Links
-----

* https://github.com/nodejs/nan/
* https://github.com/wtfil/rust-in-node
* https://github.com/fcanas/node-native-boilerplate/
* https://github.com/rustbridge/neon/
* http://www.puritys.me/docs-blog/article-286-How-to-pass-the-paramater-of-Node.js-or-io.js-into-native-C/C++-function..html
