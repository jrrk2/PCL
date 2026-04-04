cmake_minimum_required(VERSION 3.16)
project(FitsStacker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent)
set(OpenCV_DIR "/usr/lib/x86_64-linux-gnu/cmake/opencv4")
find_package(OpenCV REQUIRED COMPONENTS core imgproc features2d calib3d)
find_package(PkgConfig REQUIRED)
pkg_check_modules(CFITSIO REQUIRED cfitsio)

add_executable(fitsstacker
    main.cpp
    mainwindow.cpp
    stackengine.cpp
)

target_include_directories(fitsstacker PRIVATE
    ${CFITSIO_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(fitsstacker PRIVATE
    Qt6::Core Qt6::Widgets Qt6::Concurrent
    ${OpenCV_LIBS}
    ${CFITSIO_LIBRARIES}
)
