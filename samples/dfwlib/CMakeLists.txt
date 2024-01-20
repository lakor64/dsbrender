set(SRCS
    winwrap.c
    brwrap.h
    buffer.h
    buffer.c
    generic.c
    winincls.h
)

add_library(dfw${BRENDER_FIXED_SUFFIX} STATIC ${SRCS})
target_link_libraries(dfw${BRENDER_FIXED_SUFFIX} PUBLIC BRender::Fixed)
target_include_directories(dfw${BRENDER_FIXED_SUFFIX} PUBLIC .)
set_target_properties(dfw${BRENDER_FIXED_SUFFIX} PROPERTIES FOLDER "${BRENDER_FOLDER_PREFIX}samples")
add_library(BRender::Fixed::Sample ALIAS dfw${BRENDER_FIXED_SUFFIX})

add_library(dfw${BRENDER_FLOAT_SUFFIX} STATIC ${SRCS})
target_link_libraries(dfw${BRENDER_FLOAT_SUFFIX} PUBLIC BRender::Float)
target_include_directories(dfw${BRENDER_FLOAT_SUFFIX} PUBLIC .)
set_target_properties(dfw${BRENDER_FLOAT_SUFFIX} PROPERTIES FOLDER "${BRENDER_FOLDER_PREFIX}samples")
add_library(BRender::Float::Sample ALIAS dfw${BRENDER_FLOAT_SUFFIX})
