set(GEN_OUT "${CMAKE_CURRENT_BINARY_DIR}/generated/")

set(SRCS
    actor.h
    angles.h
    assocarr.h
    assoca_p.h
    blockops.h
    brassert.h
    brddi.h
    brddi_p.h
    brdiag.h
    brender.h
    brerror.h
    brexcept.h
    brfile.h
    brfont.h
    brhton.h
    brimage.h
    brlimits.h
    brlists.h
    brmath.h
    brmem.h
    brstdlib.h
    brv1db.h
    brvector.h
    camera.h
    colour.h
    compend.h
    brcompiler.h
    custsupt.h
    datafile.h
    dataf_p.h
    dmyifix.h
    fixed.h
    fmt.h
    fmt_p.h
    formats.h
    fw_p.h
    g386ifix.h
    host.h
    host_ip.h
    host_p.h
    lexer.h
    light.h
    material.h
    math_ip.h
    math_p.h
    math_x.h
    brmatrix.h
    model.h
    mscifix.h
    mscimath.h
    order.h
    pixelmap.h
    pm_p.h
    pool.h
    pretok.h
    proifix.h
    quat.h
    register.h
    render.h
    scalar.h
    shortcut.h
    syscn_p.h
    timing.h
    brtoken.h
    tokenmap.h
    transfrm.h
    v1db_p.h
    vecfns.h
    vecifns.h
    wtcifix.h
    ${GEN_OUT}/host.inc
    ${GEN_OUT}/dev_objs.cgh

    # brender fork
    vfx.h
    vfx_p.h
    brhalf.h
    brkeycodes.h

)

source_group("" FILES ${SRCS})

h2inc(INPUT "${CMAKE_CURRENT_LIST_DIR}/host.h" OUTPUT ${GEN_OUT}/host.inc)
classgen("${CMAKE_CURRENT_LIST_DIR}/dev_objs.hpp" ${GEN_OUT}/dev_objs.cgh)

add_library(brinc INTERFACE ${SRCS})
target_include_directories(brinc INTERFACE . ${GEN_OUT})
set_target_properties(brinc PROPERTIES FOLDER ${BRENDER_FOLDER_PREFIX}core)
