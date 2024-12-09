cmake_minimum_required(VERSION 3.18)

# set variables for source files
set(SOURCE_FILES src/main.cpp src/Game.cpp src/Game.h src/rendering/Shader.cpp src/rendering/Shader.h src/rendering/Mesh.cpp src/rendering/Mesh.h src/entities/Entity.cpp src/entities/Entity.h src/entities/Player.cpp src/entities/Player.h src/Assets.h
        src/physics/ContactListener.h
        src/entities/Platform.cpp
        src/entities/Platform.h
        src/entities/Obstacle.cpp
        src/entities/Obstacle.h
)

# set executable name
set(EXE_FILE Game)

# add the executable target
add_executable(${EXE_FILE} ${SOURCE_FILES})

# require C++ 20 compiler
target_compile_features(${EXE_FILE} PRIVATE cxx_std_20)

# link with dependencies
target_link_libraries(${EXE_FILE} PRIVATE glad glfw glm soloud box2d)

target_compile_definitions(${EXE_FILE} PRIVATE ASSET_ROOT=./assets/)

# copy assets to output directory before build
add_custom_command(TARGET ${EXE_FILE} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${EXE_FILE}>/assets)
