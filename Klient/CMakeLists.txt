cmake_minimum_required(VERSION 3.18)
project(klient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)
set(SDL2_PATH "C:/Users/kubov/Desktop/SDL2-2.28.5/x86_64-w64-mingw32")


find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})



add_executable(klient main.cpp)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ws2_32.lib)




