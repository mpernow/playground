#pragma once

#include <opencv2/core.hpp>
#include <vector>

namespace calib {

// Homography mapping objectPts to imagePts, estimated with the normalized DLT
cv::Matx33d estimateHomography(const std::vector<cv::Point2d> &objectPts,
                               const std::vector<cv::Point2d> &imagePts);

// Mean distance in pixels between H * objectPts and imagePts
double reprojectionErrorPx(const cv::Matx33d &H,
                           const std::vector<cv::Point2d> &objectPts,
                           const std::vector<cv::Point2d> &imagePts);

} // namespace calib
