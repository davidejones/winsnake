cmake_minimum_required(VERSION 3.6)
set(PROJ_NAME winsnake)

project(${PROJ_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if (MINGW)
    # This fixed "The program can't start because libgcc_s_dw2-1.dll is missing from your computer."
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
    # reduce the size by stripping symbols, only in release mode?
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")
endif()

set(LIBS ${LIBS} user32.lib gdi32.lib)

set(SOURCE_FILES main.cpp)
add_executable(winsnake ${SOURCE_FILES})

target_link_libraries(${PROJ_NAME} ${LIBS})