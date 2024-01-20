set(SRCS
    WINVFXHC.c
    WINVFX16.asm
    #WINVFX8.asm
)

br_core_target(NAME brvfx SRCS ${SRCS} NO_DIVISION)
