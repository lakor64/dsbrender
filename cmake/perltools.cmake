find_package(Perl REQUIRED)

set(H2_INC_COMMON "${CMAKE_CURRENT_LIST_DIR}/../core/inc" "${CMAKE_CURRENT_BINARY_DIR}/core/inc/generated")

# tools
set(INFOGEN "${CMAKE_CURRENT_LIST_DIR}/infogen.pl")
set(MKDRV "${CMAKE_CURRENT_LIST_DIR}/mkdrv.pl") # https://github.com/crocguy0688/CrocDE-BRender/blob/master/cmake/h2inc.cmake
set(CLASSGEN "${CMAKE_CURRENT_LIST_DIR}/classgen.pl")
set(TOKGEN "${CMAKE_CURRENT_LIST_DIR}/tokgen.pl")

function(h2inc)
    cmake_parse_arguments(H2 "" "INPUT;OUTPUT" "INCS" ${ARGN})
    set(TARGET_INCLUDES ${H2_INC_COMMON} ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES} ${H2_INCS})
    add_custom_command(
        OUTPUT ${H2_OUTPUT}
        COMMAND ${PERL_EXECUTABLE} ${MKDRV} "$<$<BOOL:${TARGET_INCLUDES}>:/I$<JOIN:${TARGET_INCLUDES},;/I>>" ${H2_INPUT} ${H2_OUTPUT} "${CMAKE_CURRENT_BINARY_DIR}/"
        MAIN_DEPENDENCY ${H2_INPUT}
        DEPENDS ${MKDRV}
        COMMAND_EXPAND_LISTS
    )
endfunction()

function(infogen)
    cmake_parse_arguments(IG "FLOAT;FIXED;NORMAL" "INPUT;OUTPUT" "ARGS" ${ARGN})

    if (IG_FLOAT)
        set(CUSTOM_ARGS "image_suffix=f float_components")
    elseif(IG_FIXED)
        set(CUSTOM_ARGS "image_suffix=x")
    endif()
    
    add_custom_command(
        OUTPUT ${IG_OUTPUT}
        MAIN_DEPENDENCY ${IG_INPUT}
        DEPENDS ${INFOGEN}
        COMMAND ${PERL_EXECUTABLE} ${INFOGEN} ${CUSTOM_ARGS} ${IG_ARGS} < ${IG_INPUT} > ${IG_OUTPUT}
    )
endfunction()

function(classgen INPUT_FILE OUTPUT_FILE)
    # https://github.com/crocguy0688/CrocDE-BRender/blob/f170811b5c2633b4c18367851e2c8b3fdd88c85f/core/fw/CMakeLists.txt#L22
    add_custom_command(
            OUTPUT ${OUTPUT_FILE}
            # This will work with MSVC and GCC
            COMMAND "${CMAKE_C_COMPILER}" -D__CLASSGEN__ -E "${INPUT_FILE}" > "${CMAKE_CURRENT_BINARY_DIR}/_classgen.tmp"
            COMMAND "${PERL_EXECUTABLE}" "${CLASSGEN}" < "${CMAKE_CURRENT_BINARY_DIR}/_classgen.tmp" > ${OUTPUT_FILE}
            MAIN_DEPENDENCY ${INPUT_FILE}
    )
endfunction()

function(pretok INPUT_FILE HEADER_OUT C_OUT TYPE_OUT)
    add_custom_command(
            # TODO: 
            # OUTPUT "${HEADER_OUT}" "${C_OUT}" "${TYPE_OUT}"
            # COMMAND "${PERL_EXECUTABLE}" "${TOKGEN}" "${INPUT_FILE}" "${HEADER_OUT}" "${C_OUT}" "${TYPE_OUT}"
            OUTPUT pretok.h pretok.c toktype.c
            COMMAND "${PERL_EXECUTABLE}" "${TOKGEN}" "${INPUT_FILE}"
            MAIN_DEPENDENCY "${INPUT_FILE}"
            DEPENDS "${TOKEN}"
    )
endfunction()
