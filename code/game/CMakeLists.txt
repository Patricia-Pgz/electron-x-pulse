cmake_minimum_required(VERSION 3.18)

# Set source directory
set(GAME_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Glob all .cpp and .h files for the game
file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS
        "${GAME_SRC_DIR}/*.cpp"
        "${GAME_SRC_DIR}/*.h"
)

# Name of the executable
set(EXE_FILE ElectronXPulse)

# Create the executable from all game source files
add_executable(${EXE_FILE} ${GAME_SRC})

# Includes and dependencies
target_include_directories(${EXE_FILE} PUBLIC ${GAME_SRC_DIR})
target_compile_features(${EXE_FILE} PUBLIC cxx_std_20)
target_link_libraries(${EXE_FILE}
        PRIVATE
        Electrine
        nlohmann_json::nlohmann_json
        glaze::glaze
)

# Copy assets to output dir before launch
add_custom_command(TARGET ${EXE_FILE} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${EXE_FILE}>/assets
)
