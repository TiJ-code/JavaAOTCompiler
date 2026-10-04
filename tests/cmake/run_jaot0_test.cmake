if(NOT DEFINED JAOT0)
    message(FATAL_ERROR "JAOT0 was not provided")
endif()

if(NOT DEFINED INPUT)
    message(FATAL_ERROR "INPUT was not provided")
endif()

if(NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "OUTPUT_DIR was not provided")
endif()

if(NOT DEFINED CXX)
    message(FATAL_ERROR "CXX was not provided")
endif()

if(NOT DEFINED RUNTIME)
    message(FATAL_ERROR "RUNTIME was not provided")
endif()

if(NOT DEFINED LAUNCHER)
    message(FATAL_ERROR "LAUNCHER was not provided")
endif()

file(MAKE_DIRECTORY "${OUTPUT_DIR}")

get_filename_component(PROGRAM_NAME "${INPUT}" NAME_WE)

set(ASM "${OUTPUT_DIR}/${PROGRAM_NAME}.s")
set(OBJ "${OUTPUT_DIR}/${PROGRAM_NAME}.o")
set(EXECUTABLE "${OUTPUT_DIR}/${PROGRAM_NAME}")

message(STATUS "JAOT0 compiling ${INPUT}")

# .jaot -> .s
execute_process(
        COMMAND
        "${JAOT0}"
        "${INPUT}"
        -S
        -o "${ASM}"
        RESULT_VARIABLE COMPILE_RESULT
        OUTPUT_VARIABLE COMPILE_OUTPUT
        ERROR_VARIABLE COMPILE_ERROR
)

if(NOT COMPILE_RESULT EQUAL 0)
    message(FATAL_ERROR
            "jaot0 failed for ${INPUT}\n"
            "stdout:\n${COMPILE_OUTPUT}\n"
            "stderr:\n${COMPILE_ERROR}"
    )
endif()

if(NOT EXISTS "${ASM}")
    message(FATAL_ERROR
            "jaot0 reported success but did not create ${ASM}"
    )
endif()

message(STATUS "Generated ${ASM}")

# .s -> .o
execute_process(
        COMMAND
        "${CXX}"
        -c
        "${ASM}"
        -o "${OBJ}"
        RESULT_VARIABLE ASSEMBLE_RESULT
        OUTPUT_VARIABLE ASSEMBLE_OUTPUT
        ERROR_VARIABLE ASSEMBLE_ERROR
)

if(NOT ASSEMBLE_RESULT EQUAL 0)
    message(FATAL_ERROR
            "Generated assembly failed to assemble\n"
            "stdout:\n${ASSEMBLE_OUTPUT}\n"
            "stderr:\n${ASSEMBLE_ERROR}"
    )
endif()

message(STATUS "Assembled ${OBJ}")

# .o + launcher + runtime -> executable
execute_process(
        COMMAND
        "${CXX}"
        "${OBJ}"
        "${LAUNCHER}"
        "${RUNTIME}"
        -o "${EXECUTABLE}"
        RESULT_VARIABLE LINK_RESULT
        OUTPUT_VARIABLE LINK_OUTPUT
        ERROR_VARIABLE LINK_ERROR
)

if(NOT LINK_RESULT EQUAL 0)
    message(FATAL_ERROR
            "Failed to link ${PROGRAM_NAME}\n"
            "stdout:\n${LINK_OUTPUT}\n"
            "stderr:\n${LINK_ERROR}"
    )
endif()

message(STATUS "Linked ${EXECUTABLE}")

# run generated program
execute_process(
        COMMAND
        "${EXECUTABLE}"
        RESULT_VARIABLE RUN_RESULT
        OUTPUT_VARIABLE RUN_OUTPUT
        ERROR_VARIABLE RUN_ERROR
)

if(NOT RUN_RESULT EQUAL 0)
    message(FATAL_ERROR
            "Generated program ${PROGRAM_NAME} failed\n"
            "exit code: ${RUN_RESULT}\n"
            "stdout:\n${RUN_OUTPUT}\n"
            "stderr:\n${RUN_ERROR}"
    )
endif()

message(STATUS "Executed ${PROGRAM_NAME}")
message(STATUS "Program output:")
message(STATUS "${RUN_OUTPUT}")
