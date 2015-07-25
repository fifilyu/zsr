PROJECT(zsr)

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" is_system_dir)
IF("${is_system_dir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${is_system_dir}" STREQUAL "-1")

ADD_EXECUTABLE(tool_zsr 
    tool_zsr.c
    framework.c
    module/mod_cpu.c
    module/mod_dns.c
    module/mod_fs.c
    module/mod_gateway.c
    module/mod_io.c
    module/mod_ip.c
    module/mod_load_avg.c
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
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
