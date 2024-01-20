set(SRCS
    # OBJS_C
    divtrap.c
    dossetup.c
    eventq.c
    keyboard.c
    readmse.c

    # OBJS_ASM
    clock.asm
    divtraph.asm
    eventq.inc
    scancode.inc
    rmeventq.inc
    #rmeventq.asm
    #kbdint.asm
)

set(CFLAGS -D__BR_V1DB__=0)
br_core_target(NAME brdi SRCS ${SRCS} CFLAGS ${CFLAGS} NO_DIVISION)
