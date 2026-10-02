#pragma once

#include <opencv2/core.hpp>

namespace calib {

struct Intrinsics {
  double fx, fy, cx, cy;
  cv::Matx33d matrix() const {
    return cv::Matx33d(fx, 0, cx, 0, fy, cy, 0, 0, 1);
  }
};

// Project a point on the board plane (Z = 0 in world coordinates) to pixel
// coordinates. R is rotation, t is translation (extrinsics)
cv::Point2d projectPoint(const Intrinsics &A, const cv::Matx33d &R,
                         const cv::Vec3d &t, const cv::Point2d &Xw);

} // namespace calib
