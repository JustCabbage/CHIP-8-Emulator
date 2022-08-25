cmake_minimum_required(VERSION 3.0.0)
project(CHIP-8-Emulator VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Dependencies)
add_executable(CHIP-8-Emulator main.cpp Core/Emulator/CPU.cpp)
target_link_libraries(CHIP-8-Emulator PRIVATE ImGui-SFML::ImGui-SFML)