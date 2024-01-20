set(SRCS
    # OBJS_C
    angles.c
    fixed.c
    matrix23.c
    matrix34.c
    matrix4.c
    plane.c
    quat.c
    scalar.c
    transfrm.c
    vector.c
    # OBJS_ASM
    fixed386.asm
    fxadc386.asm
)

br_core_target(NAME brmt SRCS ${SRCS})
