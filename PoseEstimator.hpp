/*
Component: PoseEstimator
Primary owner: Abdulmajid
Checkpoint: [Checkpoint]
Responsibility: Estimate the marker's 3D position and distance from the camera.
Main contributions: Position estimation using marker corners and camera calibration, and distance calculation.
*/

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <optional>
#include <vector>

// Innehåller resultatet från positionsberäkningen.
struct Pose
{
    cv::Vec3d position; // Markerns centrum: X, Y och Z i meter.
};

class PoseEstimator
{
public:
    // Skapar en PoseEstimator med kamerans kalibreringsdata.
    PoseEstimator(const cv::Mat &cameraMatrix,
                  const cv::Mat &distortionCoefficients);

    // Beräknar markerns 3D-position relativt kameran.
    // imageCorners: fyra hörn runt markerns kant i originalbilden,
    // i motsvarande ordning som 3D-hörnen i implementationen.
    // Hörnen ska beskriva den verkliga markern, utan extra förstoring.
    // markerSize: markerns verkliga sidlängd i meter.
    // Kalibreringen måste motsvara bildens upplösning och koordinater.
    // Returnerar en Pose om beräkningen lyckas, annars std::nullopt.
    std::optional<Pose> estimatePose(
        const std::vector<cv::Point2f> &imageCorners,
        double markerSize) const;

    // Beräknar det raka 3D-avståndet till markerns centrum i meter.
    double distanceToMarker(const Pose &pose) const;

private:
    cv::Mat cameraMatrix_;
    cv::Mat distortionCoefficients_;
};