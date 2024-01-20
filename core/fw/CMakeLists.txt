set(SRCS
    # OBJS_C
    brbegin.c
    brbhook.c
    brlists.c
    brqsort.c
    bswap.c
    datafile.c
    devlist.c
    devsetup.c
    diag.c
    error.c
    file.c
    fwsetup.c
    genfile.c
    image.c
    lexer.c
    loader.c
    mem.c
    object.c
    objectc.c
    pattern.c
    pool.c
    printf.c
    register.c
    resource.c
    resreg.c
    scratch.c
    token.c
    tokenval.c
    assocarr.c
    sys_conf.c
    devquery.c
    wincoff.h
    fwimg.h
    fw_ix.h
    fw_ip.h
    fw.h
    # OBJ_ASM
    blockops.asm
    magicsym.asm
)


set(CFLAGS -D__BR_V1DB__=0)
br_core_target(NAME brfw SRCS ${SRCS} CFLAGS ${CFLAGS})
