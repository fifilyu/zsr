
PROJECT(libzsr)

ADD_LIBRARY(zsr SHARED 
    common.c
    log.c
    cpu.c
    dns.c
    fs.c
    gateway.c
    io.c
    ip.c
    load_avg.c
    mem.c
    network.c
    proc.c
    swap.c
    time.c
    uptime.c
    who.c
)

INSTALL(TARGETS zsr
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

INSTALL(DIRECTORY ../include/zsr DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
