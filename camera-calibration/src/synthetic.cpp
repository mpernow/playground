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
                                     int minVisible, unsigned seed,
                                     double noiseStd) {
  CV_Assert(minVisible >= 4 && minVisible <= board.cols * board.rows);

  std::mt19937 rng(seed);
  std::uniform_real_distribution<double> tiltDeg(15.0, 45.0);
  std::uniform_real_distribution<double> axisComponent(-1.0, 1.0);
  std::uniform_real_distribution<double> depthMm(500.0, 900.0);
  std::uniform_real_distribution<double> lateralMm(-150.0, 150.0);

  CV_Assert(noiseStd >= 0.0);
  std::normal_distribution<double> noise(0.0, 1.0);

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

    // Project the object points to the image, and keep those that are within
    // bounds
    BoardView view{R, t, {}, {}};
    for (const auto &Xw : objectPoints) {
      cv::Point2d p = projectPoint(A, R, t, Xw);
      cv::Point2d pNoisy = {p.x + noiseStd * noise(rng),
                            p.y + noiseStd * noise(rng)};
      if (p.x < 20 || p.x > imageSize.width - 20 || p.y < 20 ||
          p.y > imageSize.height - 20) {
        continue;
      }
      view.imagePoints.push_back(pNoisy);
      view.objectPoints.push_back(Xw);
    }
    // Only keep this board image if enough points are within bounds
    if (static_cast<int>(view.imagePoints.size()) < minVisible)
      continue;

    views.push_back(std::move(view));
  }
  return views;
}

} // namespace calib
