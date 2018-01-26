
cmake_minimum_required(VERSION 2.8)
 
PROJECT(Compute_octree_from_STL)
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
 
add_executable(Compute_octree_from_STL MACOSX_BUNDLE Compute_octree_from_STL.cxx )
 
target_link_libraries(Compute_octree_from_STL ${VTK_LIBRARIES})
