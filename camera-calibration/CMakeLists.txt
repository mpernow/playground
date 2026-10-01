cmake_minimum_required(VERSION 3.10)
project(camera_calibration CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED COMPONENTS core calib3d)

add_executable(camera_calibration src/main.cpp)
target_link_libraries(camera_calibration ${OpenCV_LIBS})
