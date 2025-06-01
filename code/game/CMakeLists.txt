cmake_minimum_required(VERSION 3.18)

# Set source directory
set(GAME_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Glob all .cpp and .h files
file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS
        "${GAME_SRC_DIR}/*.cpp"
        "${GAME_SRC_DIR}/*.h"
)

# Create a static library for the game logic
add_library(game-lib STATIC ${GAME_SRC})
target_include_directories(game-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(game-lib PUBLIC cxx_std_20)
target_link_libraries(game-lib PUBLIC Electrine)

# Now create the standalone game executable
set(EXE_FILE ElectronXPulse)
add_executable(${EXE_FILE} src/main.cpp)
target_link_libraries(${EXE_FILE} PRIVATE game-lib)

# Copy assets to the output directory
add_custom_command(TARGET ${EXE_FILE} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${EXE_FILE}>/assets)
