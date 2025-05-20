cmake_minimum_required(VERSION 3.18)

add_executable(LevelEditor
        src/main.cpp
        src/LevelEditor.cpp
        src/LevelEditor.h
)

target_include_directories(LevelEditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(LevelEditor PRIVATE cxx_std_20)

# Link against game logic and engine
target_link_libraries(LevelEditor PRIVATE game-lib Electrine)

# Copy assets
add_custom_command(TARGET LevelEditor PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:LevelEditor>/assets)