cmake_minimum_required(VERSION 3.4.0)
project(minild66)

if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")
include_directories(src/include)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wno-comment")
set(PROJECT_LINK_LIBS m JsonBox)

set(EXECUTABLE_NAME "minild66")
add_executable(${EXECUTABLE_NAME} ${SOURCES})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2.3 COMPONENTS system graphics window network)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIRS})
    set(PROJECT_LINK_LIBS ${PROJECT_LINK_LIBS} ${SFML_LIBRARIES})
else()
    message(FATAL_ERROR "SFML not found, CMake will exit.")
endif()

target_link_libraries(${EXECUTABLE_NAME} ${PROJECT_LINK_LIBS})

install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
