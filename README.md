# wobble utility code repository

wobble is a repository of utility code to be included in other projects.

The repository has these rules:

 - all code is unit tested
 - all header files are thoroughly documented
 - it must be possible to perform a simple search and replace of the strings
   'WOBBLE' and 'wobble', so that the code can be deployed to fit as part of
   another library.
 - APIs are stable. Also ABIs, whenever possible.
 - code only depends on C++11 standard library features
 - it must be easier to maintain wobble code than it is to manage dependencies
   on an ecosystem of libraries that provide the same funtionality

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
