cmake_minimum_required(VERSION 3.18)


set(ENGINE_NAME Electrine)


file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "include/**.h")
file(GLOB_RECURSE PRIVATE_HEADER_LIST CONFIGURE_DEPENDS "src/**.h")
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "src/**.cpp")

add_library(${ENGINE_NAME} ${HEADER_LIST} ${PRIVATE_HEADER_LIST} ${SOURCE_LIST}
)

target_include_directories(${ENGINE_NAME} PUBLIC include/)
include_directories(${CMAKE_SOURCE_DIR}/aubio)

# link with dependencies
set(AUBIO_LIB_PATH ${CMAKE_SOURCE_DIR}/extern/aubio/build/src/aubio.lib)
target_link_libraries(${ENGINE_NAME} PUBLIC glad glfw glm soloud box2d ${AUBIO_LIB_PATH} EnTT::EnTT)



target_compile_features(${ENGINE_NAME} PRIVATE cxx_std_20)
set_target_properties(${ENGINE_NAME} PROPERTIES LINKER_LANGUAGE CXX)