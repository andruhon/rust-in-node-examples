Rust in node examples
=====================

Experimenting with adding Rust extension into NodeJS/V8 crossplatformingly
via C++ native addon with help of C ABI interface.

Entirely the same approach works for Electron framework, with a little nuance
that you should put your plugin into node_modules and do `electron-rebuild.cmd -w youraddonname -f`

Experimented parameter and return types:

void, bool, c_int, c_double (JS float), *c_char (JS string), Struct (JS object), *c_int (JS numeric array)

What's inside:

* Rust dynamic library in ./src/embed.rs (linked with mylib.cc)
* Native NodeJS C++ addon ./src/addon.cc using ./src/embed.rs library

Tested on Windows, Ubuntu and OS X


Installing
----------
NON npm dependencies:

* node-gyp `installation recommendations <https://github.com/nodejs/node-gyp#installation>`_ fulfilled properly properly for your platform
* Rust and Cargo (I did not check older version, but I think it should work with Rust 1.6+). I, personally use `multirust-rs <https://github.com/Diggsey/multirust-rs>`_ and nightly target

NPM dependencies:

  > npm install


Building
--------
Build whole addon:

  > npm run build

Rebuild Rust only (keep addon.node as it is)

  > npm run rebuild-rust

Please refer to `package.json <package.json>`_ for more details.


Running
------

  > node index.js


Links
-----

* https://github.com/nodejs/nan/
* https://github.com/wtfil/rust-in-node
* https://github.com/fcanas/node-native-boilerplate/
* https://github.com/rustbridge/neon/
* https://github.com/andruhon/rust-in-node-cpp-back-example
* http://www.puritys.me/docs-blog/article-286-How-to-pass-the-paramater-of-Node.js-or-io.js-into-native-C/C++-function..html
* https://github.com/nodejs/nan/tree/master/examples/async_pi_estimate
* https://blog.scottfrees.com/c-processing-from-node-js-part-4-asynchronous-addons

Disclaimer
----------

These examples are possibly unsafe due to my insufficient competence in subject.
However they should be useful for beginners.
