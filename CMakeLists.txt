cmake_minimum_required(VERSION 3.21)
project(MyOpenCVProject)

# Create Main project
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -g -I../../thirdparty/boostlibs")

# ここに.cppファイル
set(SOURCES
    main.cpp
    ./camera/capture.cpp
)

# ここに.hファイル
set(HEADERS
    # main.h
)

# Mainという実行ファイルを生成
add_executable(Main
    ${SOURCES}
    ${HEADERS}
)


# ライブラリをinclude
find_package(OpenCV REQUIRED)
target_link_libraries(Main ${OpenCV_LIBS})
find_package(Threads REQUIRED)
target_link_libraries(Main Threads::Threads)
find_package(OpenGL REQUIRED)
target_link_libraries(Main ${OpenGL_LIBS})
