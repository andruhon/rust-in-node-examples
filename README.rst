Rust in node examples
=====================

Experimenting with adding Rust extension into NodeJS via C++ addon

Work still in progress

List of available functions:

* int to int function
* string to string function
* numeric array to numeric array function
* void to struct as an object
* struct as object to bool
* callbacks

What's inside:

* Rust dynamic library in ./src/embed.rs (linked with mylib.cc)
* Native NodeJS C++ addon ./src/addon.cc using ./src/embed.rs library

Tested on Windows, Ubuntu and OS X


Installing
----------
  Install dependencies:

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
* http://www.puritys.me/docs-blog/article-286-How-to-pass-the-paramater-of-Node.js-or-io.js-into-native-C/C++-function..html
* https://github.com/nodejs/nan/tree/master/examples/async_pi_estimate
* https://blog.scottfrees.com/c-processing-from-node-js-part-4-asynchronous-addons

Disclaimer
----------

These examples are possibly unsafe due to my insufficient competence in subject.
However they should be useful for beginners.
