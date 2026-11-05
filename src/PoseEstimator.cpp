/*
Component: PoseEstimator
Primary owner: Abdulmajid
Responsibility: Estimate the marker's 3D position and distance from the camera.
Main contributions: Position estimation using marker corners and camera calibration, and distance calculation.
*/

#include "PoseEstimator.hpp"
#include <cmath>

// Initializes PoseEstimator and copies the camera calibration data.
PoseEstimator::PoseEstimator(const cv::Mat &cameraMatrix,
                             const cv::Mat &distortionCoefficients)
{
    cameraMatrix_ = cameraMatrix.clone();
    distortionCoefficients_ = distortionCoefficients.clone();
}

// Estimates the marker's 3D position relative to the camera.
// Returns std::nullopt if the number of corners or marker size is invalid,
// or if solvePnP fails.
// Assumes valid camera calibration and image corner coordinates.
std::optional<Pose> PoseEstimator::estimatePose(
    const std::vector<cv::Point2f> &imageCorners,
    double markerSize) const
{
    // A square marker must have exactly four corners.
    if (imageCorners.size() != 4)
    {
        return std::nullopt;
    }

    // The marker size must be finite and greater than zero.
    // The size is specified in metres.
    if (!std::isfinite(markerSize) || markerSize <= 0.0)
    {
        return std::nullopt;
    }

    double halfSize = markerSize / 2.0;

    // Define the marker corners in its local 3D coordinate system.
    // The marker is flat and centered at (0, 0, 0).
    // Image corners must follow the same order as these object points.
    std::vector<cv::Point3f> objectPoints = {
        cv::Point3f(-halfSize, halfSize, 0.0),
        cv::Point3f(halfSize, halfSize, 0.0),
        cv::Point3f(halfSize, -halfSize, 0.0),
        cv::Point3f(-halfSize, -halfSize, 0.0)};

    // solvePnP calculates the rotation internally.
    // The rotation is not converted to degrees or returned.
    cv::Vec3d rotationVector;

    // Stores the marker center position relative to the camera.
    cv::Vec3d translationVector;

    bool success = cv::solvePnP(
        objectPoints,
        imageCorners,
        cameraMatrix_,
        distortionCoefficients_,
        rotationVector,
        translationVector);

    if (!success)
    {
        return std::nullopt;
    }

    // The position uses the same unit as markerSize: metres.
    Pose pose;
    pose.position = translationVector;

    return pose;
}

// Restores the original marker corners before estimating its 3D position.
std::optional<Pose> PoseEstimator::estimatePoseFromScaledCorners(
    const std::vector<cv::Point2f> &scaledCorners,
    double markerSize) const
{
    // A square marker must have exactly four corners.
    if (scaledCorners.size() != 4)
    {
        return std::nullopt;
    }

    std::vector<cv::Point2f> corners = scaledCorners;

    // Calculate the center of the marker.
    const cv::Point2f center =
        (corners[0] + corners[1] +
         corners[2] + corners[3]) *
        0.25f;

    // Reverse the scaling applied by MarkerRecognizer.
    for (auto &corner : corners)
    {
        corner = center + (corner - center) / 1.5f;
    }

    // Estimate the pose using the original corner positions.
    return estimatePose(corners, markerSize);
}

// Calculates the straight-line 3D distance to the marker center in metres.
double PoseEstimator::distanceToMarker(const Pose &pose) const
{
    double x = pose.position[0];
    double y = pose.position[1];
    double z = pose.position[2];

    return std::sqrt(x * x + y * y + z * z);
}