#include "homography.hpp"

#include <cmath>

namespace calib {

namespace {

cv::Matx33d normalizingTransform(const std::vector<cv::Point2d> &pts) {
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

std::vector<cv::Point2d> applyTransform(const cv::Matx33d &T,
                                        const std::vector<cv::Point2d> &pts) {

  std::vector<cv::Point2d> out;
  out.reserve(pts.size());
  for (const auto &p : pts) {
    cv::Vec3d h = T * cv::Vec3d(p.x, p.y, 1.0);
    out.emplace_back(h[0] / h[2], h[1] / h[2]);
  }
  return out;
}

} // namespace

cv::Matx33d estimateHomography(const std::vector<cv::Point2d> &objectPts,
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

double reprojectionErrorPx(const cv::Matx33d &H,
                           const std::vector<cv::Point2d> &objectPts,
                           const std::vector<cv::Point2d> &imagePts) {
  double errSum = 0;
  for (size_t i = 0; i < objectPts.size(); ++i) {
    cv::Point3d mappedPt = H * objectPts[i];
    errSum += std::sqrt((mappedPt.x / mappedPt.z - imagePts[i].x) *
                            (mappedPt.x / mappedPt.z - imagePts[i].x) +
                        (mappedPt.y / mappedPt.z - imagePts[i].y) *
                            (mappedPt.y / mappedPt.z - imagePts[i].y));
  }
  return errSum / objectPts.size();
}

} // namespace calib
