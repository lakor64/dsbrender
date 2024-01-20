set(SRCS
    # OBJS_C
    himage.c
    hook.c
    hostcfg.c
    hstsetup.c
    memmgmt.c
    ports.c
    hostimg.h
    hwindow.c
    hmouse.c
    # OBJS_ASM
    cpuid.asm
    farmem.asm
    prot.asm
    real.asm
    realmem.asm
    segregs.asm
    dpmi.inc
    pl.inc
    # brender fork
    hstvk.c
    hstvk.h
)

set(CFLAGS -D__BR_V1DB__=0)

if (BRENDER_DOS)
    set(CFLAGS ${CFLAGS} -D__DOS__=1)
elseif(BRENDER_WIN32)
    set(CFLAGS ${CFLAGS} -D__WIN_32__=1)
endif()

if ("${BRENDER_DOS_EXTENDER}" STREQUAL "PHARRLAP") # TNT DOS Extender
    set(CFLAGS ${CFLAGS} -D__PHARLAP386__=1)
elseif ("${BRENDER_DOS_EXTENDER}" STREQUAL "X32" OR "${BRENDER_DOS_EXTENDER}" STREQUAL "X32VM")
   set(CFLAGS ${CFLAGS} -D__X32__=1)
elseif ("{BRENDER_DOS_EXTENDER}" STREQUAL "DOS4GW" OR "{BRENDER_DOS_EXTENDER}" STREQUAL "DOS4GWPRO") # DOS/4GW (included in owcc)
    set(CFLAGS ${CFLAGS} -D__DPMI__=1 -D__DOS4G__=1)
elseif ("${BRENDER_DOS_EXTENDER}" STREQUAL "POWERPACK") # Borland DOS Extender
    set(CFLAGS ${CFLAGS} -D__DPMI__=1 -D__POWERPACK__=1)
elseif ("${BRENDER_DOS_EXTENDER}" STREQUAL "GO32") # FreePascal DOS Extender
    set(CFLAGS ${CFLAGS} -D__DPMI__=1 -D__GO32__=1)
endif()

br_core_target(NAME brhs SRCS ${SRCS} CFLAGS ${CFLAGS} NO_DIVISION)
