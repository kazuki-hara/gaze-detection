cmake_minimum_required(VERSION 3.21)
project(FoveCameraControl)

# Create Main project
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -g -I../../thirdparty/boostlibs --libs libv4l2")

# ここに.cppファイル
set(SOURCES
    ./utils.cpp
    ./utils_main.cpp
)

# ここに.hファイル
set(HEADERS
    ./utils.h
)

# Mainという実行ファイルを生成
add_executable(main
    ${SOURCES}
    ${HEADERS}
)


# ライブラリをinclude
find_package(OpenCV REQUIRED)
target_link_libraries(main ${OpenCV_LIBS})
find_package(Threads REQUIRED)
target_link_libraries(main Threads::Threads)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries(main ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )