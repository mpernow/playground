#include "synthetic.hpp"

#include <opencv2/calib3d.hpp>
#include <random>

namespace calib {

std::vector<cv::Point2d> boardObjectPoints(const Board &board) {
  std::vector<cv::Point2d> pts;
  pts.reserve(board.cols * board.rows);

  for (int j = 0; j < board.rows; ++j) {
    for (int i = 0; i < board.cols; ++i) {
      double X = (i - (board.cols - 1) / 2.0) * board.squareSize;
      double Y = (j - (board.rows - 1) / 2.0) * board.squareSize;
      pts.emplace_back(X, Y);
    }
  }
  return pts;
}

std::vector<BoardView> generateViews(const Intrinsics &A, cv::Size imageSize,
                                     const Board &board, int count,
                                     unsigned seed) {
  std::mt19937 rng(seed);
  std::uniform_real_distribution<double> tiltDeg(15.0, 45.0);
  std::uniform_real_distribution<double> axisComponent(-1.0, 1.0);
  std::uniform_real_distribution<double> depthMm(500.0, 900.0);
  std::uniform_real_distribution<double> lateralMm(-150.0, 150.0);

  auto objectPoints = boardObjectPoints(board);
  std::vector<BoardView> views;

  while (static_cast<int>(views.size()) < count) {
    // Construct random rotation and translations
    cv::Vec3d axis(axisComponent(rng), axisComponent(rng), axisComponent(rng));
    double norm = cv::norm(axis);
    if (norm < 1e-6)
      // Discard transformations that are nearly parallel to the camera
      continue;
    axis *= (1.0 / norm) * (tiltDeg(rng) * CV_PI / 180.0);
    cv::Matx33d R;
    cv::Rodrigues(axis, R);
    cv::Vec3d t(lateralMm(rng), lateralMm(rng), depthMm(rng));

    // Project the object points to the image, and keep them only if they are
    // all within bounds
    std::vector<cv::Point2d> imgPts;
    bool inBounds = true;
    for (const auto &Xw : objectPoints) {
      cv::Point2d p = projectPoint(A, R, t, Xw);
      if (p.x < 20 || p.x > imageSize.width - 20 || p.y < 20 ||
          p.y > imageSize.height - 20) {
        inBounds = false;
        break;
      }
      imgPts.push_back(p);
    }
    if (!inBounds)
      continue;

    views.push_back({R, t, imgPts});
  }
  return views;
}

} // namespace calib
