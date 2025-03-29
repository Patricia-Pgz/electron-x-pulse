cmake_minimum_required(VERSION 3.18)

# set variables for source files
set(SOURCE_FILES src/main.cpp src/Game.cpp src/Game.h src/Constants.h src/Assets.h
        src/physics/ContactListener.h
        src/physics/PhysicsSystem.h
)

# set executable name
set(EXE_FILE ElectronXPulse)

# add the executable target
add_executable(${EXE_FILE} ${SOURCE_FILES})

# require C++ 20 compiler
target_compile_features(${EXE_FILE} PRIVATE cxx_std_20)

target_link_libraries(${EXE_FILE} PRIVATE Electrine)


target_compile_definitions(${EXE_FILE} PRIVATE ASSET_ROOT=${CMAKE_SOURCE_DIR}/assets)

# copy assets to output directory before build
add_custom_command(TARGET ${EXE_FILE} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${EXE_FILE}>/assets)
