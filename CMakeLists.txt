project(vis2b)
cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(PackageRegistry)

find_package(dyn2b)

include_directories(
  include
  ${dyn2b_INCLUDE_DIRS}
)

add_subdirectory(src)


install(
  DIRECTORY include/
  DESTINATION include
  FILES_MATCHING PATTERN "*.h"
)

register_package(
  INCLUDE_DIRS
    include
  LIBRARIES
    ${EXPORTED_LIBRARIES}
)
