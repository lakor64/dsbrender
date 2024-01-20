add_executable(minzb WIN32 minimal.c)
set_target_properties(minzb PROPERTIES FOLDER "${BRENDER_FOLDER_PREFIX}samples")
target_link_libraries(minzb PRIVATE BRender::Fixed::Sample BRender::Fixed::SoftRender BRender::Fixed::SoftPrimitive)
