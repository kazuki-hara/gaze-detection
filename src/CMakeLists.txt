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
    ./cammount/cammount.cpp
    ./fove/opengl.cpp
)

# ここに.hファイル
set(HEADERS
    ./camera/camera.h
    ./gaze/gaze.h
    ./fove/display.h
    ./main.h
    ./utils/utils.h
    ./calibration/calibration.h
    ./cammount/cammount.h
    ./fove/opengl.h
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
if(OPENGL_FOUND)
    message("OpenGL found")
else()
    message("OpenGL not found")
endif()

find_package(GLUT REQUIRED)
if(GLUT_FOUND)
    message("GLUT found")
else()
    message("GLUT not found")
endif()

include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})
target_link_libraries(main ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

