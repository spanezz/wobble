project(wobble)

include(CheckCXXSourceCompiles)
check_cxx_source_compiles(
 "#include <dirent.h>
  int main() {
      struct dirent *d;
      (void)d->d_type;
      return 0;
  }" HAVE_STRUCT_DIRENT_D_TYPE)

add_definitions( --std=c++11 -Wall -Werror -DHAVE_CONFIG_H -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 )

# glob through source files/headers
file(GLOB WOBBLE_HEADERS *.h)
file(GLOB WOBBLE_SOURCES *.cc)

include_directories(${wobble_SOURCE_DIR}/.. ${wobble_BINARY_DIR}/..)
add_definitions(${OPT_FLAGS})

add_executable(wobble-test ${WOBBLE_SOURCES})
add_test(wobble-test wobble-test)
add_dependencies(check wobble-test)

