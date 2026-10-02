#include "homography.hpp"
#include "synthetic.hpp"

#include <iostream>
#include <opencv2/calib3d.hpp>
#include <vector>

using namespace calib;

static const Intrinsics kTrueIntrinsics{920.0, 915.0, 640.0, 360.0};
static const cv::Size kImageSize(1280, 720);
static const Board kBoard{9, 6, 24.0};

int main() {
  auto objectPts = boardObjectPoints(kBoard);
  auto views = generateViews(kTrueIntrinsics, kImageSize, kBoard, 15,
                             /*seed=*/42);
  std::cout << "Generated " << views.size() << " views of a " << kBoard.cols
            << "x" << kBoard.rows << " board.\n\n";

  const BoardView &view0 = views[0];
  cv::Matx33d Hdlt = estimateHomography(objectPts, view0.imagePoints);

  std::vector<cv::Point2f> objF, imgF;
  for (const auto &p : objectPts) {
    objF.emplace_back(p.x, p.y);
  }
  for (const auto &p : view0.imagePoints) {
    imgF.emplace_back(p.x, p.y);
  }
  cv::Mat Hcv = cv::findHomography(objF, imgF, 0); // 0 means no RANSAC

  std::cout << "Our H (view 0):\n" << cv::Mat(Hdlt) / Hdlt(2, 2) << "\n\n";
  std::cout << "cv::findHomography (view 0):\n"
            << Hcv / Hcv.at<double>(2, 2) << "\n\n";

  double ourErr = reprojectionErrorPx(Hdlt, objectPts, view0.imagePoints);
  double cvErr =
      reprojectionErrorPx(cv::Matx33d(Hcv), objectPts, view0.imagePoints);

  std::cout << "Mean reprojection error, our DLT:      " << ourErr << " px\n";
  std::cout << "Mean reprojection error, cv::findHomography: " << cvErr
            << " px\n";
  return 0;
}
