find_package(PkgConfig)
pkg_check_modules(OPENSCENEGRAPH openscenegraph)

include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

add_library(vis2b_osg SHARED osg.cpp)
target_link_libraries(vis2b_osg PRIVATE ${OPENSCENEGRAPH_LIBRARIES})


install(
  TARGETS vis2b_osg
  EXPORT vis2b_osg-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

set(EXPORTED_LIBRARIES ${EXPORTED_LIBRARIES} vis2b_osg PARENT_SCOPE)
