cmake_minimum_required(VERSION 3.18)


set(ENGINE_NAME Electrine)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/extern/imgui)
set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

set(IMGUI_HEADERS
        ${IMGUI_DIR}/imgui.h
        ${IMGUI_DIR}/imgui_internal.h
        ${IMGUI_DIR}/imconfig.h
        ${IMGUI_DIR}/backends/imgui_impl_glfw.h
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
)
set(AUBIO_LIB_PATH ${CMAKE_SOURCE_DIR}/extern/aubio/build/src/aubio.lib)

file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "include/**.h")
file(GLOB_RECURSE PRIVATE_HEADER_LIST CONFIGURE_DEPENDS "src/**.h")
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "src/**.cpp")

add_library(${ENGINE_NAME} ${HEADER_LIST} ${PRIVATE_HEADER_LIST} ${SOURCE_LIST}
        ${IMGUI_SOURCES}
)

target_include_directories(${ENGINE_NAME} PUBLIC include/
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends)
include_directories(${CMAKE_SOURCE_DIR}/aubio)

# link with dependencies
target_link_libraries(${ENGINE_NAME} PUBLIC glad glfw glm soloud box2d ${AUBIO_LIB_PATH} EnTT::EnTT nlohmann_json::nlohmann_json glaze::glaze)

target_compile_definitions(${ENGINE_NAME} PRIVATE ASSET_ROOT=${CMAKE_SOURCE_DIR}/assets)

target_compile_features(${ENGINE_NAME} PRIVATE cxx_std_20)
set_target_properties(${ENGINE_NAME} PROPERTIES LINKER_LANGUAGE CXX)