cmake_minimum_required(VERSION 3.10)
project(FileManager)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS filesystem)

add_executable(FileManager FileManager.cpp)

target_link_libraries(FileManager Boost::filesystem)