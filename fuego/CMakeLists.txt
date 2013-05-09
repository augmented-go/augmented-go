FILE(GLOB fuego_go "go/*.cpp" "go/*.h")
source_group(go FILES ${fuego_go})

FILE(GLOB fuego_gtpengine "gtpengine/*.cpp" "gtpengine/*.h")
source_group(gtpengine FILES ${fuego_gtpengine})

FILE(GLOB fuego_sg "smartgame/*.cpp" "smartgame/*.h")
source_group(smartgame FILES ${fuego_sg})

include_directories(go smartgame gtpengine)

find_package(Boost 1.52 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
add_library(fuego ${fuego_go} ${fuego_sg} ${fuego_gtpengine})