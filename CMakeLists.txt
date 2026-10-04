cmake_minimum_required(VERSION 3.20)

project(jaot
        VERSION 0.1.0
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(JAOT_WARNINGS_AS_ERRORS
        "Treat compiler warnings as errors"
        OFF
)

add_executable(jaot0)

target_sources(jaot0
        PRIVATE
        bootstrap/jaot0/src/main.cpp
        bootstrap/jaot0/src/lexer.cpp
        bootstrap/jaot0/src/parser.cpp
        bootstrap/jaot0/src/codegen.cpp
)

target_include_directories(jaot0
        PRIVATE
        bootstrap/jaot0/include
)

if (MSVC)
    target_compile_options(jaot0 PRIVATE /W4)

    if (JAOT_WARNINGS_AS_ERRORS)
        target_compile_options(jaot0 PRIVATE /WX)
    endif()
else()
    target_compile_options(jaot0 PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )

    if (JAOT_WARNINGS_AS_ERRORS)
        target_compile_options(jaot0 PRIVATE -Werror)
    endif()
endif()

add_library(jaot_runtime STATIC)

target_sources(jaot_runtime
        PRIVATE
        runtime/runtime.cpp
)

target_include_directories(jaot_runtime
        PUBLIC
        runtime
)

enable_testing()

add_test(
        NAME jaot0_help
        COMMAND jaot0 --help
)