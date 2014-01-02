cmake_minimum_required(VERSION 2.8)

# Set the default CMAKE_BUILD_TYPE to Release.
# This should be done before the project command since the latter can set
# CMAKE_BUILD_TYPE itself (it does so for nmake).
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
endif ()

project(RSTPARSER)

add_library(rstparser rstparser.cc rstparser.h)
if (NOT MSVC)
  set(EXTRA_FLAGS "-Wall -Wextra -pedantic")
endif ()

set_target_properties(rstparser PROPERTIES COMPILE_FLAGS ${EXTRA_FLAGS})

# We compile Google Test ourselves instead of using pre-compiled libraries.
# See the Google Test FAQ "Why is it not recommended to install a
# pre-compiled copy of Google Test (for example, into /usr/local)?"
# at http://code.google.com/p/googletest/wiki/FAQ for more details.
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/gtest/CMakeLists.txt)
  option(
    gtest_force_shared_crt
    "Use shared (DLL) run-time lib even when Google Test is built as static lib."
    ON)
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE=1)
  endif ()
  add_subdirectory(gtest)
  include_directories(gtest/include)
  link_directories(${CMAKE_CURRENT_BINARY_DIR}/gtest)
  enable_testing()
  add_executable(rstparser-test rstparser-test.cc)
  target_link_libraries(rstparser-test rstparser gtest)
  set_target_properties(rstparser-test PROPERTIES COMPILE_FLAGS ${EXTRA_FLAGS})
  add_test(rstparser-test rstparser-test)
endif ()
