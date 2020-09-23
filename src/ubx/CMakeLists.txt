find_package(PkgConfig)

pkg_check_modules(UBX REQUIRED ubx0)

pkg_get_variable(UBX_MODDIR ${UBX_MODULE_NAME} UBX_MODDIR)
pkg_get_variable(UBX_EXEC_PREFIX ${UBX_MODULE_NAME} exec_prefix)

file(RELATIVE_PATH UBX_MODDIR_REL ${UBX_EXEC_PREFIX} ${UBX_MODDIR})

include_directories(${UBX_INCLUDE_DIRS})
link_directories(${UBX_LIBRARY_DIRS})

add_library(vis2b_ubx SHARED vis2b_osg_ubx.c)
target_link_libraries(vis2b_ubx PRIVATE vis2b_osg ${UBX_LIBRARIES})

configure_file(ubx_env.sh.in ${CMAKE_BINARY_DIR}/ubx_env.sh)

add_custom_command(TARGET vis2b_ubx
  POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_CURRENT_BINARY_DIR}/libvis2b_ubx.so"
      "${CMAKE_CURRENT_BINARY_DIR}/${UBX_MODDIR_REL}/vis2b.so")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libvis2b_ubx.so"
        DESTINATION "${CMAKE_INSTALL_DIRECTOR}${UBX_MODDIR_REL}"
        RENAME vis2b.so)
