cmake_minimum_required(VERSION 3.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_BUILD_TYPE Release)

project(eccodesTest)

add_executable(${PROJECT_NAME} ./main.cpp)

target_include_directories(${PROJECT_NAME}
    PUBLIC ./include
)

target_link_libraries(${PROJECT_NAME} PUBLIC 
${CMAKE_SOURCE_DIR}/lib/eccodes.lib
)