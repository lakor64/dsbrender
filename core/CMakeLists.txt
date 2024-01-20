# Variables will be setted up by the projgen
set(BRENDER_CORE_NORMAL_TARGETS )
set(BRENDER_CORE_FIXED_TARGETS )
set(BRENDER_CORE_FLOAT_TARGETS )

if (NOT BRENDER_STATIC) # for dll export
    set(BRENDER_CORE_SRCS dll/brender.def dll/dllmain.c)
    set(BRENDER_FIXED_CORE_SRCS dll/fixed.def)
    set(BRENDER_FLOAT_CORE_SRCS dll/float.def)

    if (BRENDER_DOS)
        list(APPEND BRENDER_CORE_SRCS dll/dos.def)
    endif()
else()
    set(BRENDER_CORE_SRCS dll/dllmain.c)
endif()

source_group("" FILES ${BRENDER_CORE_SRCS} ${BRENDER_FIXED_CORE_SRCS} ${BRENDER_FLOAT_CORE_SRCS})

if (NOT DEFINED BRENDER_CORE_NAME)
    set(BRENDER_CORE_NAME brnd)
endif()

# Targets
add_subdirectory(inc)
add_subdirectory(fw)
add_subdirectory(host)
add_subdirectory(std)
add_subdirectory(pixelmap)

if (BRENDER_DOS)
    add_subdirectory(dosio)
endif()

add_subdirectory(v1db)
add_subdirectory(math)
add_subdirectory(fmt)
add_subdirectory(vfx)

# Main BRender library

# float
add_library(${BRENDER_CORE_NAME}${BRENDER_FLOAT_SUFFIX} ${BRENDER_LIB_TYPE} ${BRENDER_CORE_SRCS} ${BRENDER_FLOAT_CORE_SRCS})
target_link_libraries(${BRENDER_CORE_NAME}${BRENDER_FLOAT_SUFFIX} ${BRENDER_LINK_TYPE} ${BRENDER_CORE_NORMAL_TARGETS} ${BRENDER_CORE_FLOAT_TARGETS} PUBLIC brinc)
set_target_properties(${BRENDER_CORE_NAME}${BRENDER_FLOAT_SUFFIX} PROPERTIES FOLDER ${BRENDER_FOLDER_PREFIX})
# fixed
add_library(${BRENDER_CORE_NAME}${BRENDER_FIXED_SUFFIX} ${BRENDER_LIB_TYPE} ${BRENDER_CORE_SRCS} ${BRENDER_FIXED_CORE_SRCS})
target_link_libraries(${BRENDER_CORE_NAME}${BRENDER_FIXED_SUFFIX} ${BRENDER_LINK_TYPE} ${BRENDER_CORE_NORMAL_TARGETS} ${BRENDER_CORE_FIXED_TARGETS} PUBLIC brinc)
set_target_properties(${BRENDER_CORE_NAME}${BRENDER_FIXED_SUFFIX} PROPERTIES FOLDER ${BRENDER_FOLDER_PREFIX})

# aliases
add_library(BRender::Fixed ALIAS ${BRENDER_CORE_NAME}${BRENDER_FIXED_SUFFIX})
add_library(BRender::Float ALIAS ${BRENDER_CORE_NAME}${BRENDER_FLOAT_SUFFIX})

