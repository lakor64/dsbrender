set(GEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/generated/)

set(SRCS
    # OBJS_C
    alpha.c
    clip.c
    convert.c
    device.c
    driver.c
    faceops.c
    genrend.c
    gprim.c
    gv1buckt.c
    gv1model.c
    heap.c
    light24.c 
    light8.c
    mapping.c
    object.c
    onscreen.c
    renderer.c
    rendfcty.c
    rndstate.c
    setup.c
    sstate.c
    state.c
    v1model.c
    lighting.c
    depth.c

    # OBJS_ASM
    geometry.asm
    magicsym.asm
    #face_386.asm
    subdiv.asm
    cull.asm
    gen_386.asm
    #v1m_386.asm
    # EXPORT_INCLUDES
    priminfo.h
    gen_386.h
    device.h
    face_386.h
    gv1model.h
    gv1buckt.h
    gprim.h
    lightmac.h
    lighting.h
    pro_epi.inc
    rend.h
    renderer.h
    rendfcty.h
    sstate.h
    state.h
    template.h
    timestmp.h
    v1m_386.h
    vertex.inc
    zsrmacro.h
	${GEN_OUT}/drv.inc
)

set(FIXED_SRCS
    v1m_386.asm
)

set(SHARED_SRCS softrend.def)

h2inc(INPUT "${CMAKE_CURRENT_LIST_DIR}/drv.h" OUTPUT "${GEN_OUT}/drv.inc")
br_driver_target(NAME softrnd ALIAS SoftRender EXPORT BrDrv1SoftRendBegin SRCS ${SRCS} SHARED_SRCS ${SHARED_SRCS} INCS ${GEN_OUT} FIXED_SRCS ${FIXED_SRCS})
