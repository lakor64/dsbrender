set(SRCS
    stddiag.c
    stdmem.c
    stdfile.c
    stdlib.c
    math.c
    logwrite.c
    brexcept.c
)

set(CFLAGS -D__BR_V1DB__=0)

br_core_target(NAME brst SRCS ${SRCS} CFLAGS ${CFLAGS} NO_DIVISION)
