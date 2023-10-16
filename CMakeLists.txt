cmake_minimum_required(VERSION 3.26.4)
Project(ChatPlays)
message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(CMAKE_BUILD_TYPE Debug)

find_package(Threads)
find_package(cpprestsdk REQUIRED)
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON) # Without this flag CMake may resort to just '-lpthread'


if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
    find_package(PkgConfig REQUIRED)

    pkg_search_module(EVDEV REQUIRED libevdev)
    set (SOURCES src/Linux/Uinput.cpp src/main.cpp src/twitch.cpp 
    src/Linux/LinuxSock.cpp src/Settings/Settings.cpp src/Settings/Oauth.cpp src/PineClient.cpp src/Command.cpp 
    src/Buffer.cpp)

    add_executable(ChatPlays ${SOURCES})
    target_include_directories(ChatPlays PRIVATE ${CMAKE_SOURCE_DIR}/Header ${CMAKE_SOURCE_DIR}/Header/Settings ${CMAKE_SOURCE_DIR}/3rdparty ${EVDEV_INCLUDE_DIRS})
    target_link_libraries(ChatPlays PRIVATE ${EVDEV_LIBRARIES} Threads::Threads cpprestsdk::cpprest OpenSSL::Crypto)
endif()

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
    
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
    add_subdirectory(3rdparty/windows/ViGEmClient)

    set (SOURCES src/Windows/Xinput.cpp src/main.cpp src/twitch.cpp src/Windows/winConnect.cpp src/Settings/Settings.cpp src/Settings/Oauth.cpp src/PineClient.cpp src/message.cpp)
    add_executable(ChatPlays ${SOURCES})
    target_link_libraries(ChatPlays PRIVATE Threads::Threads ViGEmClient wsock32 ws2_32 cpprestsdk::cpprest OpenSSL::Crypto)
    target_include_directories(ChatPlays PRIVATE ${CMAKE_SOURCE_DIR}/Header ${CMAKE_SOURCE_DIR}/Header/Settings ${CMAKE_SOURCE_DIR}/3rdparty)
endif()

