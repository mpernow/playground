#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <random>
#include <vector>

struct Intrinsics {
  double fx, fy, cx, cy;
  cv::Matx33d matrix() const {
    return cv::Matx33d(fx, 0, cx, 0, fy, cy, 0, 0, 1);
  }
};

static const Intrinsics kTrueIntrinsics{920.0, 915.0, 640.0, 360.0};
static const cv::Size kImageSize(1280, 720);

static const int kBoardCols = 9;
static const int kBoardRows = 6;
static const double kSquareSize = 24.0; // mm

static std::vector<cv::Point2d> boardObjectPoints() {
  std::vector<cv::Point2d> pts;
  pts.reserve(kBoardCols * kBoardRows);

  for (int j = 0; j < kBoardRows; ++j) {
    for (int i = 0; i < kBoardCols; ++i) {
      double X = (i - (kBoardCols - 1) / 2.0) * kSquareSize;
      double Y = (j - (kBoardRows - 1) / 2.0) * kSquareSize;
      pts.emplace_back(X, Y);
    }
  }
  return pts;
}

// Transform a world coordinate point Xw to camera coordinates
// R is rotation, t is traslation (extrinsics)
static cv::Point2d projectBoardPoint(const Intrinsics &A, const cv::Matx33d &R,
                                     const cv::Vec3d &t,
                                     const cv::Point2d &Xw) {
  cv::Vec3d Pw(Xw.x, Xw.y, 0.0);
  cv::Vec3d Pc = R * Pw + t;
  double xp = Pc[0] / Pc[2];
  double yp = Pc[1] / Pc[2];
  return cv::Point2d(A.fx * xp + A.cx, A.fy * yp + A.cy);
}

struct BoardView {
  cv::Matx33d R;
  cv::Vec3d t;
  std::vector<cv::Point2d> imagePoints;
};

static std::vector<BoardView> generateViews(int count, unsigned seed) {
  std::mt19937 rng(seed);
  std::uniform_real_distribution<double> tiltDeg(15.0, 45.0);
  std::uniform_real_distribution<double> axisComponent(-1.0, 1.0);
  std::uniform_real_distribution<double> depthMm(500.0, 900.0);
  std::uniform_real_distribution<double> lateralMm(-150.0, 150.0);

  auto objectPoints = boardObjectPoints();
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

    // Transform the object points to camera coordinates, and keep them only if
    // they are all within bounds
    std::vector<cv::Point2d> imgPts;
    bool inBounds = true;
    for (const auto &Xw : objectPoints) {
      cv::Point2d p = projectBoardPoint(kTrueIntrinsics, R, t, Xw);
      if (p.x < 20 || p.x > kImageSize.width - 20 || p.y < 20 ||
          p.y > kImageSize.height - 20) {
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

namespace dlt {

static cv::Matx33d normalizingTransform(const std::vector<cv::Point2d> &pts) {
  cv::Point2d centroid(0, 0);
  for (const auto &p : pts) {
    centroid += p;
  }
  centroid *= (1.0 / pts.size());

  double meanDist = 0.0;
  for (const auto &p : pts) {
    meanDist += cv::norm(p - centroid);
  }
  meanDist *= (1.0 / pts.size());

  double scale = std::sqrt(2.0) / meanDist;
  return cv::Matx33d(scale, 0, -scale * centroid.x, 0, scale,
                     -scale * centroid.y, 0, 0, 1);
}

static std::vector<cv::Point2d>
applyTransform(const cv::Matx33d &T, const std::vector<cv::Point2d> &pts) {

  std::vector<cv::Point2d> out;
  out.reserve(pts.size());
  for (const auto &p : pts) {
    cv::Vec3d h = T * cv::Vec3d(p.x, p.y, 1.0);
    out.emplace_back(h[0] / h[2], h[1] / h[2]);
  }
  return out;
}

static cv::Matx33d
estimateHomography(const std::vector<cv::Point2d> &objectPts,
                   const std::vector<cv::Point2d> &imagePts) {
  CV_Assert(objectPts.size() == imagePts.size() && objectPts.size() >= 4);

  cv::Matx33d Tobj = normalizingTransform(objectPts);
  cv::Matx33d Timg = normalizingTransform(imagePts);
  auto objN = applyTransform(Tobj, objectPts);
  auto imgN = applyTransform(Timg, imagePts);

  int n = static_cast<int>(objN.size());
  cv::Mat A = cv::Mat::zeros(2 * n, 9, CV_64F);
  for (int i = 0; i < n; ++i) {
    double x = objN[i].x, y = objN[i].y;
    double u = imgN[i].x, v = imgN[i].y;

    A.at<double>(2 * i, 0) = -x;
    A.at<double>(2 * i, 1) = -y;
    A.at<double>(2 * i, 2) = -1;

    A.at<double>(2 * i, 6) = u * x;
    A.at<double>(2 * i, 7) = u * y;
    A.at<double>(2 * i, 8) = u;

    A.at<double>(2 * i + 1, 3) = -x;
    A.at<double>(2 * i + 1, 4) = -y;
    A.at<double>(2 * i + 1, 5) = -1;

    A.at<double>(2 * i + 1, 6) = v * x;
    A.at<double>(2 * i + 1, 7) = v * y;
    A.at<double>(2 * i + 1, 8) = v;
  }

  cv::Mat w, u, vt;
  cv::SVD::compute(A, w, u, vt, cv::SVD::FULL_UV);
  cv::Mat h = vt.row(
      vt.rows -
      1); // Right singular vector corresponding to smallest singular value

  cv::Matx33d Hn(h.at<double>(0), h.at<double>(1), h.at<double>(2),
                 h.at<double>(3), h.at<double>(4), h.at<double>(5),
                 h.at<double>(6), h.at<double>(7), h.at<double>(8));

  return Timg.inv() * Hn * Tobj;
}

} // namespace dlt

double reprojectionErrorPx(const cv::Matx33d &H,
                           const std::vector<cv::Point2d> &objectPts,
                           const std::vector<cv::Point2d> &imagePts) {
  double errSum = 0;
  for (int i = 0; i < objectPts.size(); ++i) {
    cv::Point3d mappedPt = H * objectPts[i];
    errSum += std::sqrt((mappedPt.x / mappedPt.z - imagePts[i].x) *
                            (mappedPt.x / mappedPt.z - imagePts[i].x) +
                        (mappedPt.y / mappedPt.z - imagePts[i].y) *
                            (mappedPt.y / mappedPt.z - imagePts[i].y));
  }
  return errSum / objectPts.size();
}

int main() {
  auto objectPts = boardObjectPoints();
  auto views = generateViews(15, /*seed=*/42);
  std::cout << "Generated " << views.size() << " views of a " << kBoardCols
            << "x" << kBoardRows << " board.\n\n";

  const BoardView &view0 = views[0];
  cv::Matx33d Hdlt = dlt::estimateHomography(objectPts, view0.imagePoints);

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
