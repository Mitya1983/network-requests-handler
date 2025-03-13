cmake_minimum_required(VERSION 3.17)

project(test LANGUAGES CXX DESCRIPTION "Test executable for manual testing")


include_directories(
    ${CMAKE_SOURCE_DIR}
)

file(GLOB_RECURSE INC_FILES CONFIGURE_DEPENDS
        ${PROJECT_SOURCE_DIR}/inc/*.hpp
)
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
        ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_executable(${PROJECT_NAME} main.cpp)