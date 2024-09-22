cmake_minimum_required(VERSION 3.30.3)
project(cHTTP)

add_executable(cHTTP main.c)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(cHTTP PRIVATE DEV_MODE)
endif()
