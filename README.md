# silentloop
Node.js-like, toy event loop written on C++.

This is an experimental project that was created because of my personal interest in seeing how well (or not) some  popular programming principles from JavaScript world, in particular from Node.js, would look from the perspective of relatively modern C++ dialect.

Most of the design concepts for this project has been inspired by Node.js [documentation](https://nodejs.org/api).

***

## Demo application

### Build
```
$ mkdir build
$ cd build
$ cmake ..
$ make -j `nproc`
```

### Run
```
$ ./silentloop
```
