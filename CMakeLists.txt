cmake_minimum_required(VERSION 3.20)

project(
  JAOT
  VERSION 0.1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQURIED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(JAOT_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

add_executable(JAOT0 bootstrap/jaot0/src/main.cpp)

target_include_directories(JAOT0 PRIVATE bootstrap/jaot0/include)

if(MSVC)
  target_compile_options(jaot0 PRIVATE /W4)

  if(JAOT_WARNINGS_AS_ERRORS)
    target_compile_options(jaot0 PRIVATE /WX)
  endif()
else()
  target_compile_options(jaot0 PRIVATE -Wall -Wextra -Wpedantic)

  if(JAOT_WARNINGS_AS_ERRORS)
    target_compile_options(jaot0 PRIVATE -Werror)
  endif()
endif()

# runtime here

enable_testing()

add_test(NAME jaot0_help COMMAND jaot0 --help)
