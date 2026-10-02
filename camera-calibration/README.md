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
./build/homography_demo
```

## Layout

- [camera.hpp](src/camera.hpp) — `Intrinsics`, pinhole projection
- [synthetic.hpp](src/synthetic.hpp) — `Board`, synthetic board views
- [homography.hpp](src/homography.hpp) — normalized DLT, reprojection error
- [homography_demo.cpp](src/homography_demo.cpp) — DLT vs. `cv::findHomography`
