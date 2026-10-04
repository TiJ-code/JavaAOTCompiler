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

set(JAOT0_TEST_OUTPUT_DIR
        "${CMAKE_CURRENT_BINARY_DIR}/tests/programs/jaot0"
)

add_test(
        NAME jaot0_compile_hello
        COMMAND
        ${CMAKE_COMMAND}
        -DJAOT0=$<TARGET_FILE:jaot0>
        -DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/tests/programs/jaot0/hello.jaot
        -DOUTPUT_DIR=${JAOT0_TEST_OUTPUT_DIR}
        -DCXX=${CMAKE_CXX_COMPILER}
        -DRUNTIME=$<TARGET_FILE:jaot_runtime>
        -DLAUNCHER=${CMAKE_CURRENT_SOURCE_DIR}/tests/runtime/launcher.cpp
        -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/run_jaot0_test.cmake
)

add_test(
        NAME jaot0_compile_functions
        COMMAND
        ${CMAKE_COMMAND}
        -DJAOT0=$<TARGET_FILE:jaot0>
        -DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/tests/programs/jaot0/functions.jaot
        -DOUTPUT_DIR=${JAOT0_TEST_OUTPUT_DIR}
        -DCXX=${CMAKE_CXX_COMPILER}
        -DRUNTIME=$<TARGET_FILE:jaot_runtime>
        -DLAUNCHER=${CMAKE_CURRENT_SOURCE_DIR}/tests/runtime/launcher.cpp
        -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/run_jaot0_test.cmake
)

set_tests_properties(
        jaot0_compile_hello
        jaot0_compile_functions
        PROPERTIES
        DEPENDS jaot0_help
)