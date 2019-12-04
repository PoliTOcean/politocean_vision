cmake_minimum_required(VERSION 3.5)
project(PolitoceanVision)

set(CMAKE_CXX_STANDARD 11)

if("${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
	In-source builds are not allowed.
	Please, create a separate directory for build files (e.g. build/).
    Delete CMakeCache.txt, make a separate dir and try again.
    ")
endif()

find_package(OpenCV REQUIRED)

include_directories(include)

add_executable(Line src/main.cpp)

target_link_libraries(Line ${OpenCV_LIBS})