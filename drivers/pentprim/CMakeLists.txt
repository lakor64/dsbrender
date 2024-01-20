set(GEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/generated/)

set(INCS
    ${GEN_OUT}
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../softrend
)

set(ASM_SRCS
	magicsym.asm

	safediv.asm
	sar16.asm
	decal.asm

	t_piza.asm
	ti8_piz.asm
	ti8_pizp.asm
	t15_pizp.asm
	tt15_piz.asm
	tt16_piz.asm
	tt24_piz.asm

	t_pia.asm
	ti8_pi.asm
	ti8_pip.asm
	t15_pip.asm
	tt15_pi.asm
	tt16_pi.asm
	tt24_pi.asm

	zb8.asm
	zb8sh.asm
	zb8p2lit.asm
	zb8p2fla.asm
	zb8p2ulb.asm
	zb8p2ltb.asm
	zb8p2flb.asm
	zb8p2unl.asm
	zb8awtm.asm
	zs8.asm
	zs8p2unl.asm
	zs8awtm.asm
	fpsetup.asm
	fti8_piz.asm

	zb16p2ul.asm

	pfpsetup.asm
	fti8pizp.asm
	fti8_pip.asm
)

set(SRCS
    awtm.c
    awtmi.h
    awtmz.c
    device.c
    device.h
    dither.c
    driver.c
    drv.h
    drv_ip.h
    fperspi.h
    fperspzi.h
    frcp.c
    l_pi.c
    l_piz.c
    match.c
    match.h
    mmx_ip.h
    object.c
    object.h
    persp.c
    perspi.h
    perspz.c
    perspzi.h
    plib.c
    plib.h
    p_pi.c
    p_piz.c
    prim_ip.h
    pstate.c
    pstate.h
    rastparm.h
    rcp.c
    sbuffer.c
    sbuffer.h
    template.h
    timestmp.h
    work.c
    work.h
)

set(PRIMFILES
    prim_l15
    prim_l16
    prim_l24
    prim_l8
    prim_p15
    prim_p16
    prim_p24
    prim_p8
    prim_t24
)

set(PRIMFILES_FX
    prim_t8
    prm_t15
    prm_t16
)

set(FIXED_SRCS )
set(FLOAT_SRCS )

set(SHARED_SRCS softprim.def)
set(CFLAGS -DPARTS=0xffff -DAUTOLOAD=0)
set(ASM_CFLAGS PARTS=0ffffh)

if (BRENDER_MMX)
    list(APPEND ASM_SRCS
        rastbuff.asm
        setupdat.asm
        rastrise.asm
        xzuv.asm
        xzuvc.asm
        xzuvrgb.asm
        xzuv555.asm
        xzuvc555.asm
        xzrgb555.asm
        xzuvrgb5.asm
    )
    list(APPEND PRIMFILES_FX
        mmx_t15
        mmx_t16
    )
    list(APPEND FIXED_SRCS gsetuptx.asm)
    list(APPEND FLOAT_SRCS gsetuptx.asm)
    list(APPEND ASM_CFLAGS -DUSE_MMX=1)
endif()

# Primitive generation
foreach(PRIM IN ITEMS ${PRIMFILES})
    set_property(SOURCE match.c APPEND PROPERTY OBJECT_DEPENDS ${GEN_OUT}/${PRIM}.c)
    infogen(INPUT "${CMAKE_CURRENT_LIST_DIR}/${PRIM}.ifg" OUTPUT "${GEN_OUT}/${PRIM}.c" NORMAL)
    endforeach()
foreach (PRIM IN ITEMS ${PRIMFILES_FX})
    set_property(SOURCE match.c APPEND PROPERTY OBJECT_DEPENDS ${GEN_OUT}/${PRIM}f.c)
    set_property(SOURCE match.c APPEND PROPERTY OBJECT_DEPENDS ${GEN_OUT}/${PRIM}x.c)
    infogen(INPUT "${CMAKE_CURRENT_LIST_DIR}/${PRIM}.ifg" OUTPUT "${GEN_OUT}/${PRIM}x.c" FIXED)
    infogen(INPUT "${CMAKE_CURRENT_LIST_DIR}/${PRIM}.ifg" OUTPUT "${GEN_OUT}/${PRIM}f.c" FLOAT)
endforeach()

set_source_files_properties(${ASM_SRCS} PROPERTIES COMPILE_DEFINITIONS "${ASM_CFLAGS}")

h2inc(INPUT "${CMAKE_CURRENT_SOURCE_DIR}/drv.h" OUTPUT "${GEN_OUT}/drv.inc" INCS ${INCS})
br_driver_target(NAME softprm ALIAS SoftPrimitive EXPORT BrDrv1SoftPrimBegin CFLAGS ${CFLAGS} INCS ${INCS} SRCS ${SRCS} ${ASM_SRCS} SHARED_SRCS ${SHARED_SRCS} FIXED_SRCS ${FIXED_SRCS} FLOAT_SRCS ${FLOAT_SRCS})
