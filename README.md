# wobble utility code repository

wobble is a repository of utility code to be included in other projects.

The repository has these rules, in order of importance (early rules can
override later ones):

 - it must be possible to perform a simple search and replace of the strings
   'WOBBLE' and 'wobble', so that the code can be deployed to fit as part of
   another library.
 - code only depends on C++17 standard library features
 - old wobble code that becomes redundant because of new C++ standard features
   gets deprecated and later removed
 - APIs are stable. Also ABIs, whenever possible.
 - all code is unit tested
 - all header files are thoroughly documented
 - it must be easier to maintain wobble code than it is to manage dependencies
   on an ecosystem of libraries that provide the same funtionality

wobble does not build into a library: you just take the bits you need and use
them in your sources. Since the API is stable, you can occasionally refresh
your sources just by copying. Please contribute your fixes to wobble code back
to wobble, with unit tests.


To run wobble's unit tests:

```shell
meson setup build
meson test -vC build
```


# Changes in version 2.0

* Require C++17
* Use `std::filesystem::path` instead of `std::string` for paths. `path`
  methods have been added alongside `string` methods, which have been
  deprecated.
* `tests.h`: `ActualFile` now takes a `std::filesystem::path` and has
  path-specific tests. ABI has changed, API is still compatible
