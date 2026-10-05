foreach(required JAOT0 INPUT OUTPUT EXPECTED_ERROR)
    if(NOT DEFINED ${required})
        message(FATAL_ERROR "${required} was not provided")
    endif()
endforeach()

execute_process(
        COMMAND "${JAOT0}" "${INPUT}" -S -o "${OUTPUT}"
        RESULT_VARIABLE COMPILE_RESULT
        OUTPUT_VARIABLE COMPILE_OUTPUT
        ERROR_VARIABLE COMPILE_ERROR
)

if(COMPILE_RESULT EQUAL 0)
    message(FATAL_ERROR
            "Expected semantic analysis to reject ${INPUT}, but compilation succeeded")
endif()

string(FIND "${COMPILE_ERROR}" "${EXPECTED_ERROR}" ERROR_POSITION)
if(ERROR_POSITION EQUAL -1)
    message(FATAL_ERROR
            "Expected diagnostic '${EXPECTED_ERROR}' was not found\n"
            "stdout:\n${COMPILE_OUTPUT}\n"
            "stderr:\n${COMPILE_ERROR}")
endif()