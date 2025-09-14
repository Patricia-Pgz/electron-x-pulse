cmake_minimum_required(VERSION 3.21)
project(ElectrineEngine)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(extern/glad)
add_subdirectory(extern/glfw)
add_subdirectory(extern/glm)
add_subdirectory(extern/soloud)
add_subdirectory(extern/entt)
add_subdirectory(extern/json)
add_subdirectory(extern/glaze)
add_subdirectory(engine)

option(BOX2D_BUILD_TESTBED OFF)
option(BOX2D_BUILD_UNIT_TESTS OFF)
add_subdirectory(extern/box2d)
add_definitions(-DNOMINMAX)

add_subdirectory(game)
