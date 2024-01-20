set(LIBRARIES
    fwimg
    hostimg
    pmimg
)

set(SOURCES
    # OBJS_C
    bio.sc
    devclut.c
    device.c
    devpixmp.c
    devpml.c
    devpmw.c
    devpmwb.c
    driver.c
    object.c
    ocfree.c
    outfcty.c
    vesaapi.c
    #OBJS_ASM
    ports.asm
    vesabank.asm
    vesaloop.asm
    # INCLUDES
    drv.h
    drv_ip.h
    vesaapi.h
    vesaloop.h
    devclut.h
    devpixmp.h
    object.h
    ports.h
)

br_driver(vesa ${SOURCES} ${LIBRARIES} BrDrv1VESABegin)
