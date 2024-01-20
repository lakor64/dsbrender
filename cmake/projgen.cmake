macro(br_core_target)
    cmake_parse_arguments(CT "NO_DIVISION" "NAME;LIBTYPE" "CFLAGS;SRCS;INCS;LIBS" ${ARGN})
    source_group("" FILES ${CT_SRCS})

    if ("${CT_LIBTYPE}" STREQUAL "")
        set(CT_LIBTYPE STATIC)
    endif()

    if (CT_NO_DIVISION)
        add_library(${CT_NAME}${BRENDER_NORMAL_SUFFIX} ${CT_LIBTYPE} ${CT_SRCS})
        target_include_directories(${CT_NAME}${BRENDER_NORMAL_SUFFIX} PRIVATE ${CT_INCS} PUBLIC ${BRENDER_GLOBAL_INCLUDE})
        target_link_libraries(${CT_NAME}${BRENDER_NORMAL_SUFFIX} ${BRENDER_LINK_TYPE} ${CT_LIBS} PUBLIC brinc)
        target_compile_definitions(${CT_NAME}${BRENDER_NORMAL_SUFFIX} PRIVATE ${CT_CFLAGS})
        set_target_properties(${CT_NAME}${BRENDER_NORMAL_SUFFIX} PROPERTIES FOLDER ${BRENDER_FOLDER_PREFIX}core)

        set(BRENDER_CORE_NORMAL_TARGETS ${BRENDER_CORE_NORMAL_TARGETS} ${CT_NAME}${BRENDER_NORMAL_SUFFIX} PARENT_SCOPE)
    else()
        #fixed
        add_library(${CT_NAME}${BRENDER_FIXED_SUFFIX} ${CT_LIBTYPE} ${CT_SRCS})
        target_include_directories(${CT_NAME}${BRENDER_FIXED_SUFFIX} PRIVATE ${CT_INCS} PUBLIC ${BRENDER_GLOBAL_INCLUDE})
        target_link_libraries(${CT_NAME}${BRENDER_FIXED_SUFFIX} PRIVATE ${CT_LIBS} PUBLIC brinc)
        target_compile_definitions(${CT_NAME}${BRENDER_FIXED_SUFFIX} PRIVATE ${CT_CFLAGS} -DBASED_FIXED=1 -DBASED_FLOAT=0)
        set_target_properties(${CT_NAME}${BRENDER_FIXED_SUFFIX} PROPERTIES FOLDER ${BRENDER_FOLDER_PREFIX}core)
        #float
        add_library(${CT_NAME}${BRENDER_FLOAT_SUFFIX} ${CT_LIBTYPE} ${CT_SRCS})
        target_include_directories(${CT_NAME}${BRENDER_FLOAT_SUFFIX} PRIVATE ${CT_INCS} PUBLIC ${BRENDER_GLOBAL_INCLUDE})
        target_link_libraries(${CT_NAME}${BRENDER_FLOAT_SUFFIX} ${BRENDER_LINK_TYPE} ${CT_LIBS} PUBLIC brinc)
        target_compile_definitions(${CT_NAME}${BRENDER_FLOAT_SUFFIX} PRIVATE ${CT_CFLAGS} -DBASED_FIXED=0 -DBASED_FLOAT=1)
        set_target_properties(${CT_NAME}${BRENDER_FLOAT_SUFFIX} PROPERTIES FOLDER ${BRENDER_FOLDER_PREFIX}core)

        set(BRENDER_CORE_FLOAT_TARGETS ${BRENDER_CORE_FLOAT_TARGETS} ${CT_NAME}${BRENDER_FLOAT_SUFFIX} PARENT_SCOPE)
        set(BRENDER_CORE_FIXED_TARGETS ${BRENDER_CORE_FIXED_TARGETS} ${CT_NAME}${BRENDER_FIXED_SUFFIX} PARENT_SCOPE)
    endif()
endmacro()

macro(br_driver_target)
    cmake_parse_arguments(DR "" "NAME;ALIAS;EXPORT" "SRCS;INCS;CFLAGS;LIBS;FIXED_SRCS;FLOAT_SRCS;SHARED_SRCS" ${ARGN})

    set(ALL_SRCS ${DR_SRCS})
    set(ALL_CFLAGS ${DR_CFLAGS})
    if (NOT BRENDER_STATIC)
        list(APPEND ALL_SRCS ${DR_SHARED_SRCS})
    else()
		list(APPEND ALL_CFLAGS -DBrDrv1Begin=${DR_EXPORT})
    endif()
	
	source_group("" FILES ${ALL_SRCS})

    #fixed
    add_library(${DR_NAME}${BRENDER_FIXED_SUFFIX} ${BRENDER_LIB_TYPE} ${ALL_SRCS} ${DR_FIXED_SRCS})
    target_include_directories(${DR_NAME}${BRENDER_FIXED_SUFFIX} PRIVATE ${DR_INCS})
    target_link_libraries(${DR_NAME}${BRENDER_FIXED_SUFFIX} ${BRENDER_LINK_TYPE} ${DR_LIBS} BRender::Fixed)
    set_target_properties(${DR_NAME}${BRENDER_FIXED_SUFFIX} PROPERTIES FOLDER ${BRENDER_FOLDER_PREFIX}drivers)
    target_compile_definitions(${DR_NAME}${BRENDER_FIXED_SUFFIX} PRIVATE ${ALL_CFLAGS} PUBLIC -DBASED_FIXED=1 -DBASED_FLOAT=0)
    add_library(BRender::Fixed::${DR_ALIAS} ALIAS ${DR_NAME}${BRENDER_FIXED_SUFFIX})
    #float
    add_library(${DR_NAME}${BRENDER_FLOAT_SUFFIX} ${BRENDER_LIB_TYPE} ${ALL_SRCS} ${DR_FLOAT_SRCS})
    target_include_directories(${DR_NAME}${BRENDER_FLOAT_SUFFIX} PRIVATE ${DR_INCS})
    target_link_libraries(${DR_NAME}${BRENDER_FLOAT_SUFFIX} ${BRENDER_LINK_TYPE} ${DR_LIBS} BRender::Float)
    set_target_properties(${DR_NAME}${BRENDER_FLOAT_SUFFIX} PROPERTIES FOLDER ${BRENDER_FOLDER_PREFIX}drivers)
    target_compile_definitions(${DR_NAME}${BRENDER_FLOAT_SUFFIX} PRIVATE ${ALL_CFLAGS} PUBLIC -DBASED_FIXED=0 -DBASED_FLOAT=1)
    add_library(BRender::Float::${DR_ALIAS} ALIAS ${DR_NAME}${BRENDER_FLOAT_SUFFIX})
endmacro()
