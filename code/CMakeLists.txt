cmake_minimum_required(VERSION 3.21)
project(ElectrineEngine)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(extern/glad)
add_subdirectory(extern/glfw)
add_subdirectory(extern/glm)
add_subdirectory(extern/soloud)
add_subdirectory(extern/entt)
add_subdirectory(extern/json)
add_subdirectory(engine)

option(BOX2D_BUILD_TESTBED OFF)
option(BOX2D_BUILD_UNIT_TESTS OFF)
add_subdirectory(extern/box2d)
add_definitions(-DNOMINMAX)

file(TO_CMAKE_PATH "${CMAKE_SOURCE_DIR}/assets" ASSET_PATH)
add_compile_definitions(ASSET_ROOT=${ASSET_PATH})

add_subdirectory(game)
add_subdirectory(levelEditor)
