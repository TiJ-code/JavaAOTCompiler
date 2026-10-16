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

add_executable(jaot0
        bootstrap/jaot0/include/jaot/ir.h
        bootstrap/jaot0/include/jaot/ir_lowering.h
        bootstrap/jaot0/src/ir_lowering.cpp
        bootstrap/jaot0/include/jaot/types.h
)

target_sources(jaot0
        PRIVATE
        bootstrap/jaot0/src/main.cpp
        bootstrap/jaot0/src/lexer.cpp
        bootstrap/jaot0/src/parser.cpp
        bootstrap/jaot0/src/codegen.cpp
        bootstrap/jaot0/src/semantic.cpp
)

target_include_directories(jaot0
        PRIVATE
        bootstrap/jaot0/include
)

add_executable(jaot0_ir_header_test
        tests/ir_header_test.cpp
)

target_include_directories(jaot0_ir_header_test
        PRIVATE
        bootstrap/jaot0/include
)

add_executable(jaot0_ir_lowering_test
        tests/ir_lowering_test.cpp
        bootstrap/jaot0/src/lexer.cpp
        bootstrap/jaot0/src/parser.cpp
        bootstrap/jaot0/src/ir_lowering.cpp
)

target_include_directories(jaot0_ir_lowering_test
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

add_test(NAME jaot0_ir_header COMMAND jaot0_ir_header_test)
add_test(NAME jaot0_ir_lowering COMMAND jaot0_ir_lowering_test)

add_test(
        NAME jaot0_help
        COMMAND jaot0 --help
)

set(JAOT0_TEST_OUTPUT_DIR
        "${CMAKE_CURRENT_BINARY_DIR}/tests/programs/jaot0"
)

set(JAOT0_PROGRAM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/programs/jaot0")
# Each .jaot test requires exactly one sidecar: .expected, .error, or .runtime-fail.
file(GLOB JAOT0_PROGRAMS CONFIGURE_DEPENDS "${JAOT0_PROGRAM_DIR}/*.jaot")

foreach(JAOT0_INPUT IN LISTS JAOT0_PROGRAMS)
    get_filename_component(JAOT0_PROGRAM_NAME "${JAOT0_INPUT}" NAME_WE)
    set(JAOT0_EXPECTED_OUTPUT
            "${JAOT0_PROGRAM_DIR}/${JAOT0_PROGRAM_NAME}.expected")
    set(JAOT0_EXPECTED_ERROR
            "${JAOT0_PROGRAM_DIR}/${JAOT0_PROGRAM_NAME}.error")
    set(JAOT0_EXPECT_RUNTIME_FAILURE
            "${JAOT0_PROGRAM_DIR}/${JAOT0_PROGRAM_NAME}.runtime-fail")

    set(JAOT0_EXPECTATION_COUNT 0)
    foreach(JAOT0_EXPECTATION
            "${JAOT0_EXPECTED_OUTPUT}"
            "${JAOT0_EXPECTED_ERROR}"
            "${JAOT0_EXPECT_RUNTIME_FAILURE}")
        if(EXISTS "${JAOT0_EXPECTATION}")
            math(EXPR JAOT0_EXPECTATION_COUNT
                    "${JAOT0_EXPECTATION_COUNT} + 1")
        endif()
    endforeach()

    if(NOT JAOT0_EXPECTATION_COUNT EQUAL 1)
        message(FATAL_ERROR
                "JAOT0 test '${JAOT0_PROGRAM_NAME}' must have exactly one "
                "expectation sidecar: .expected, .error, or .runtime-fail")
    endif()

    set(JAOT0_TEST_NAME "jaot0_${JAOT0_PROGRAM_NAME}")
    set(JAOT0_TEST_COMMAND
            "${CMAKE_COMMAND}"
            "-DJAOT0=$<TARGET_FILE:jaot0>"
            "-DINPUT=${JAOT0_INPUT}"
            "-DOUTPUT_DIR=${JAOT0_TEST_OUTPUT_DIR}"
            "-DCXX=${CMAKE_CXX_COMPILER}"
            "-DRUNTIME=$<TARGET_FILE:jaot_runtime>"
            "-DLAUNCHER=${CMAKE_CURRENT_SOURCE_DIR}/tests/runtime/launcher.cpp")

    if(EXISTS "${JAOT0_EXPECTED_OUTPUT}")
        list(APPEND JAOT0_TEST_COMMAND
                "-DEXPECTED_OUTPUT_FILE=${JAOT0_EXPECTED_OUTPUT}")
    elseif(EXISTS "${JAOT0_EXPECTED_ERROR}")
        list(APPEND JAOT0_TEST_COMMAND
                "-DEXPECTED_ERROR_FILE=${JAOT0_EXPECTED_ERROR}")
    else()
        list(APPEND JAOT0_TEST_COMMAND "-DEXPECT_RUNTIME_FAILURE=ON")
    endif()

    list(APPEND JAOT0_TEST_COMMAND
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/run_jaot0_test.cmake")
    add_test(NAME "${JAOT0_TEST_NAME}" COMMAND ${JAOT0_TEST_COMMAND})
    set_tests_properties("${JAOT0_TEST_NAME}" PROPERTIES DEPENDS jaot0_help)
endforeach()