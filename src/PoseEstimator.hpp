/*
Component: PoseEstimator
Primary owner: Abdulmajid
Responsibility: Estimate the marker's 3D position and distance from the camera.
Main contributions: Position estimation using marker corners and camera calibration, and distance calculation.
*/

#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <optional>
#include <vector>

// Stores the result of the position estimation.
struct Pose
{
    cv::Vec3d position; // Marker center: X, Y and Z in metres.
};

class PoseEstimator
{
public:
    // Initializes the PoseEstimator with camera calibration data.
    PoseEstimator(const cv::Mat &cameraMatrix,
                  const cv::Mat &distortionCoefficients);

    // Estimates the marker's 3D position relative to the camera.
    // imageCorners: Four corners of the marker in the original image,
    // ordered to match the corresponding 3D object points.
    // The corners must represent the actual marker without extra scaling.
    // markerSize: Actual side length of the marker in metres.
    // Camera calibration must match the image resolution and coordinates.
    // Returns a Pose if successful, otherwise std::nullopt.
    std::optional<Pose> estimatePose(
        const std::vector<cv::Point2f> &imageCorners,
        double markerSize) const;

    // Estimates the marker pose using corners scaled by MarkerRecognizer.
    // Restores the original corner positions before estimating the pose.
    std::optional<Pose> estimatePoseFromScaledCorners(
        const std::vector<cv::Point2f> &scaledCorners,
        double markerSize) const;

    // Calculates the straight-line 3D distance to the marker center in metres.
    double distanceToMarker(const Pose &pose) const;

private:
    // Camera intrinsic matrix.
    cv::Mat cameraMatrix_;

    // Lens distortion coefficients.
    cv::Mat distortionCoefficients_;
};