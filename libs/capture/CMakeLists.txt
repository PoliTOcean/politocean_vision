cmake_minimum_required(VERSION 3.5)
project(capture VERSION 1.0.0 LANGUAGES CXX)

find_package(OpenCV REQUIRED)

add_library(capture SHARED
        capture.cpp)

add_library(PolitoceanVision::capture ALIAS capture)

target_include_directories(capture
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(capture -lpthread ${OpenCV_LIBS})

target_compile_features(capture PRIVATE cxx_auto_type)
target_compile_options(capture PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall>)

include(GNUInstallDirs)
install(TARGETS capture
        EXPORT PolitoceanVisionTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)