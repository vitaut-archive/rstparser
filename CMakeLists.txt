cmake_minimum_required(VERSION 2.8)

project(RSTPARSER)

add_library(rstparser rstparser.cc rstparser.h)

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
  add_executable(rstparser_test rstparser_test.cc)
  target_link_libraries(rstparser_test rstparser gtest)
  if (CMAKE_COMPILER_IS_GNUCXX)
    set_target_properties(rstparser_test
      PROPERTIES COMPILE_FLAGS "-Wall -Wextra -pedantic")
  endif ()
  add_test(rstparser_test rstparser_test)
endif ()
