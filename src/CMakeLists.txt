cmake_minimum_required(VERSION 3.21)
project(MyOpenCVProject)

# Create Main project
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -g -I../../thirdparty/boostlibs -lstdc++fs")

# ここに.cppファイル
set(SOURCES
    main.cpp
    ./camera/camera.cpp
    ./gaze/gaze.cpp
    ./fove/display.cpp
    #./gaze/gaze_main.cpp
    ./utils/utils.cpp
    ./calibration/calibration.cpp
)

# ここに.hファイル
set(HEADERS
    ./camera/camera.h
    ./gaze/gaze.h
    ./fove/display.h
    ./main.h
    ./utils/utils.h
    ./calibration/calibration.h
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
