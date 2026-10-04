if(NOT DEFINED JAOT0)
    message(FATAL_ERROR "JAOT0 was not provided")
endif()

if(NOT DEFINED INPUT)
    message(FATAL_ERROR "INPUT was not provided")
endif()

if(NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "OUTPUT_DIR was not provided")
endif()

if(NOT DEFINED CC)
    message(FATAL_ERROR "CC was not provided")
endif()

file(MAKE_DIRECTORY "${OUTPUT_DIR}")

get_filename_component(PROGRAM_NAME "${INPUT}" NAME_WE)

set(ASM "${OUTPUT_DIR}/${PROGRAM_NAME}.s")
set(OBJ "${OUTPUT_DIR}/${PROGRAM_NAME}.o")

message(STATUS "JAOT0 compiling ${INPUT}")

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

execute_process(
        COMMAND
        "${CC}"
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

message(STATUS "Assembly for ${PROGRAM_NAME} is valid")
