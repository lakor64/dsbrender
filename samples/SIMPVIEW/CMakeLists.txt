add_executable(simpview WIN32
    APP.c
    DISPATCH.c
    MAIN.c
    WORLD.c
    APP.h
    DISPATCH.h
    RESOURCE.h
    WORLD.h
    APP.rc
    ARGONAUT.ICO
    WIP.ICO
)

set_target_properties(simpview PROPERTIES FOLDER "${BRENDER_FOLDER_PREFIX}samples")
target_link_libraries(simpview PRIVATE BRender::Fixed::Sample BRender::Fixed::SoftRender BRender::Fixed::SoftPrimitive Winmm)
