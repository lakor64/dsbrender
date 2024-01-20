set(SRCS
    load3ds.c
    loadasc.c
    loadbmp.c
    loadgif.c
    loadiff.c
    loadnff.c
    loadscr.c
    loadtga.c
    savescr.c
    loadvue.c
    load3ds.h
)

br_core_target(NAME brfm SRCS ${SRCS})
