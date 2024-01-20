set(SRCS
    # OBJ_C
    fontptrs.c
    genclip.c
    gencopy.c
    pixelmap.c
    pmdsptch.c
    pmfile.c
    pmgen.c
    pmmem.c
    pmnull.c
    pmsetup.c
    quantize.c
    scale.c
    mipsupt.c
    fnt3x5.c
    fntp4x6.c
    fntp7x9.c
    pmimg.h
    pmmem.h
    # OBJS_ASM
    memloops.asm
)

set(CFLAGS -D__BR_V1DB__=0)

br_core_target(NAME brpm SRCS ${SRCS} CFLAGS ${CFLAGS} NO_DIVISION)
