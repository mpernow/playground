#include "camera.hpp"

namespace calib {

cv::Point2d projectPoint(const Intrinsics &A, const cv::Matx33d &R,
                         const cv::Vec3d &t, const cv::Point2d &Xw) {
  cv::Vec3d Pw(Xw.x, Xw.y, 0.0);
  cv::Vec3d Pc = R * Pw + t;
  double xp = Pc[0] / Pc[2];
  double yp = Pc[1] / Pc[2];
  return cv::Point2d(A.fx * xp + A.cx, A.fy * yp + A.cy);
}

} // namespace calib
