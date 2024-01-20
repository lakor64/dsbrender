add_definitions(-D__DRIVER__)

add_subdirectory(softrend)
add_subdirectory(pentprim)

if (BRENDER_DOS)
    add_subdirectory(vesa)
endif()
