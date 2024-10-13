cmake_minimum_required(VERSION 3.29.0)
project(cHTTP)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option (ENABLE_TESTS "Build tests with the binary" ON)

# add_executable(cHTTP main.c)
add_subdirectory(src)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(cHTTP PRIVATE DEV_MODE)
endif()

if (ENABLE_TESTS)
  find_package (Criterion REQUIRED)

  enable_testing()
  add_subdirectory(test)
endif()
