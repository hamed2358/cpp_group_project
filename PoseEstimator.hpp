/*
Component: PoseEstimator
Primary owner: Abdulmajid
Checkpoint: [Checkpoint]
Responsibility: Estimate the marker's 3D position, rotation and distance from the camera.
Main contributions: Pose estimation using marker corners and camera calibration, position and rotation calculation, and distance calculation.
*/

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <optional>

// Innehåller resultatet från pose-beräkningen.
struct Pose
{
    cv::Vec3d position; // Markerns position: X, Y och Z.
    double rotation;    // Höger/vänster-rotation i grader, -180 till 180.
};

class PoseEstimator
{
public:
    // Skapar en PoseEstimator och tar emot kamerans kalibreringsdata.
    PoseEstimator(const cv::Mat &cameraMatrix,
                  const cv::Mat &distortionCoefficients);

    // Försöker beräkna markerns position och rotation.
    // Returnerar en Pose om beräkningen lyckas, annars inget värde.
    std::optional<Pose> estimatePose(
        const std::vector<cv::Point2f> &imageCorners,
        double markerSize) const;

    // Beräknar det raka 3D-avståndet mellan kameran och markern.
    // Använder markerns X-, Y- och Z-position från en beräknad Pose.
    double distanceToMarker(const Pose &pose) const;

private:
    // Sparar kamerans interna kalibreringsvärden.
    cv::Mat cameraMatrix_;

    // Sparar information om kamerans linsförvrängning.
    cv::Mat distortionCoefficients_;
};