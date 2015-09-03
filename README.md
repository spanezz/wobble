# wobble utility code repository

wobble is a repository of utility code to be included in other sources.

The repository has these rules:

 - all code is unit tested
 - all header files are thoroughly documented
 - the strings 'WOBBLE' and 'wobble' are only ever used for namespace names, and
   it is always possible to run a simple search and replace to substitute them
   with a namespace name of your choice (like foo::utils)
 - APIs are stable
 - code only depends on C++11 standard library features

wobble does not build into a library: you just take the bits you need and use
them in your sources. Since the API is stable, you can occasionally refresh
your sources just by copying. Please contribute your fixes to wobble code back
to wobble, with unit tests.


To run wobble's unit tests:

```shell
mkdir build
cd build
# You can use "ccmake .." instead of "cmake .." to get an interactive
# interface for selecting configuration parameters.
cmake ..
make
make check
```
