cmake_minimum_required(VERSION 3.17.3)
project(ThomasTheTankEngine)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW 2.0 REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem program_options)

#internal
include_directories(../)
include_directories(../Core/CoreTests/)
include_directories(../Core/src/)
include_directories(../Core/src/Mappers/)
include_directories(../PPU/src/)
include_directories(../Main/src/)
include_directories(../Utils/)
include_directories(../libs/imgui)

#external
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS}/GL)
include_directories(${OPENGL_LIBRARIES})
message("${OPENGL_LIBRARIES}")

set(SOURCES
    ../Core/CoreTests/TestingMapper.cpp
    ../Core/src/Mapper.cpp
    ../Core/src/Core.cpp
    ../Core/src/Mappers/Mapper000.cpp
    ../Core/src/Mappers/Mapper001.cpp
    ../Utils/Utils.cpp
    ../Utils/Logger.cpp
    ../libs/imgui/imgui_widgets.cpp
    ../libs/imgui/imgui.cpp
    ../libs/imgui/imgui_impl_opengl3.cpp
    ../libs/imgui/imgui_sdl.cpp
    ../libs/imgui/imgui_demo.cpp
    ../libs/imgui/imgui_impl_sdl.cpp
    ../libs/imgui/imgui_draw.cpp
    ../PPU/src/PPU.cpp
)

add_executable(SoftDrop ../Main/src/main.cpp ${SOURCES})
target_compile_features(SoftDrop PRIVATE cxx_std_17) 

target_link_libraries(SoftDrop ${OPENGL_LIBRARIES})
target_link_libraries(SoftDrop ${SDL2_LIBRARIES})
target_link_libraries(SoftDrop ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries(SoftDrop ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(SoftDrop GLEW::GLEW)