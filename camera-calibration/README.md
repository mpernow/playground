# Camera Calibration

Camera calibration from scratch in C++, checked against OpenCV.

Synthetic views of a 9x6 checkerboard are generated from known camera
intrinsics. The board-to-image homography is then estimated with a normalized
DLT and compared with `cv::findHomography` by reprojection error.

## Build

Requires CMake, a C++17 compiler and OpenCV 4 (`core`, `calib3d`).

```sh
cmake -S . -B build
cmake --build build
```

## Run

```sh
./build/camera_calibration
```
