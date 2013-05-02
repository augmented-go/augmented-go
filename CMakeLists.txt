cmake_minimum_required (VERSION 2.6)

# The name of our project is "HELLO". CMakeLists files in this project can
# refer to the root source directory of the project as ${augmentedgo_source_dir} and
# to the root binary directory of the project as ${augmentedgo_binary_dir}.
project(AugmentedGo)

# CMAKE_BINARY_DIR: The path to the top level of the build tree.
# CMAKE_SOURCE_DIR: The path to the top level of the source tree.

add_subdirectory(Go_AugmentedReality)
add_subdirectory(Go_Backend)
add_subdirectory(Go_GUI)
add_subdirectory(Go_Scanner)
add_subdirectory(Go_VirtualReality)
