PROJECT(zsr)

ADD_EXECUTABLE(tool_zsr 
    tool_zsr.c
    framework.c
    module/mod_cpu.c
    module/mod_dns.c
    module/mod_fs.c
    module/mod_gateway.c
    module/mod_io.c
    module/mod_ip.c
    module/mod_load.c
    module/mod_mem.c
    module/mod_network.c
    module/mod_proc.c
    module/mod_swap.c
    module/mod_time.c
    module/mod_uptime.c
    module/mod_who.c
)

TARGET_LINK_LIBRARIES(tool_zsr zsr m)
SET_TARGET_PROPERTIES(tool_zsr PROPERTIES OUTPUT_NAME "zsr")

INSTALL(TARGETS tool_zsr
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
