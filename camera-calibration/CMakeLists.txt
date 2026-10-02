cmake_minimum_required(VERSION 3.10)
project(camera_calibration CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV REQUIRED COMPONENTS core calib3d)

add_library(calib src/camera.cpp src/synthetic.cpp src/homography.cpp)
target_include_directories(calib PUBLIC src)
target_link_libraries(calib PUBLIC ${OpenCV_LIBS})

add_executable(homography_demo src/homography_demo.cpp)
target_link_libraries(homography_demo PRIVATE calib)
