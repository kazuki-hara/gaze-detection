cmake_minimum_required(VERSION 2.8)
project(MyOpenCVProject)

# Create Main project
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -g -I../../thirdparty/boostlibs")


set(SOURCES
    main.cpp
    ./camera/capture.cpp
)

set(HEADERS
    # main.h
)

add_executable(Main
    ${SOURCES}
    ${HEADERS}
)


# For OpenCV
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    target_include_directories(Main PUBLIC ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(Main ${OpenCV_LIBS})
endif()
find_package(Threads REQUIRED)
target_link_libraries(Main Threads::Threads)
