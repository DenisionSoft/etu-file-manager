cmake_minimum_required(VERSION 3.10)
project(FileManager)

set(CMAKE_CXX_STANDARD 17)

# Set BOOST_ROOT if it's not automatically found
set(BOOST_ROOT $ENV{BOOST_ROOT})
set(BOOST_INCLUDEDIR ${BOOST_ROOT}/include)
set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)

find_package(Boost 1.73.0 REQUIRED COMPONENTS filesystem)

add_executable(FileManager FileManager.cpp)

target_link_libraries(FileManager Boost::filesystem)
