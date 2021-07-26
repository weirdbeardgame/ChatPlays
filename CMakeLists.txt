Project(controllerTest)
cmake_minimum_required(VERSION 3.20.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(CMAKE_BUILD_TYPE Debug)

find_package(PkgConfig REQUIRED)
pkg_search_module(EVDEV REQUIRED libevdev)

include_directories(${CMAKE_SOURCE_DIR}/Header ${EVDEV_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/3rdparty)
link_directories(${CMAKE_SOURCE_DIR}/src ${EVDEV_LIBRARIES})

set (SOURCES src/Linux/control.cpp src/main.cpp src/twitch.cpp src/Linux/connect.cpp src/settings.cpp src/ipc.cpp)
add_executable(controllerTest ${SOURCES})
target_link_libraries(controllerTest PRIVATE ${EVDEV_LIBRARIES})