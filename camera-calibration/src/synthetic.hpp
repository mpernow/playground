#pragma once

#include "camera.hpp"

#include <opencv2/core.hpp>
#include <vector>

namespace calib {

struct Board {
  int cols;
  int rows;
  double squareSize; // mm
};

// Inner corners of the board in world coordinates, centred on the origin
std::vector<cv::Point2d> boardObjectPoints(const Board &board);

struct BoardView {
  cv::Matx33d R;
  cv::Vec3d t;
  std::vector<cv::Point2d> objectPoints;
  std::vector<cv::Point2d> imagePoints;
};

// Random views of the board where enough corners land inside the image
std::vector<BoardView> generateViews(const Intrinsics &A, cv::Size imageSize,
                                     const Board &board, int count,
                                     int minVisible, unsigned seed,
                                     double noiseStd = 0.0);

} // namespace calib
