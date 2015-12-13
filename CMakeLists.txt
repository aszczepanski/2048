project(2048)
cmake_minimum_required(VERSION 2.8)

option(SET_WARNINGS "Compiler warning flags" ON)
option(SET_DEBUG "Compiler debugging flags" ON)
option(USE_STATIC_LIBS "Link with static libraries" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(EnableCXX11)
include(BuildConfig)

add_definitions(-DBOOST_ALL_NO_LIB)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.49.0 REQUIRED COMPONENTS program_options)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(
	src
	${Boost_INCLUDE_DIRS}
)

# add new directories here
add_subdirectory(src/common)
add_subdirectory(src/eval)
add_subdirectory(src/io)
add_subdirectory(src/main)
