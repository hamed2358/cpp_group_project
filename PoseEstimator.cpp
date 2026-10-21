/*
Component: PoseEstimator
Primary owner: Abdulmajid
Checkpoint: [Checkpoint]
Responsibility: Estimate the marker's 3D position and distance from the camera.
Main contributions: Position estimation using marker corners and camera calibration, and distance calculation.
*/

#include "PoseEstimator.hpp"
#include <cmath>

// Skapar PoseEstimator och kopierar kamerans kalibreringsdata.
PoseEstimator::PoseEstimator(const cv::Mat &cameraMatrix,
                             const cv::Mat &distortionCoefficients)
{
    cameraMatrix_ = cameraMatrix.clone();
    distortionCoefficients_ = distortionCoefficients.clone();
}

// Försöker beräkna markerns 3D-position relativt kameran.
// Returnerar std::nullopt vid fel antal hörn, ogiltig markörstorlek
// eller om solvePnP returnerar false.
// Förutsätter giltig kalibrering och giltiga bildhörn.
std::optional<Pose> PoseEstimator::estimatePose(
    const std::vector<cv::Point2f> &imageCorners,
    double markerSize) const
{
    // En fyrkantig markör måste ha exakt fyra hörn.
    if (imageCorners.size() != 4)
    {
        return std::nullopt;
    }

    // Markörens sidlängd ska vara ändlig och större än noll.
    // Storleken anges i meter.
    if (!std::isfinite(markerSize) || markerSize <= 0.0)
    {
        return std::nullopt;
    }

    double halfSize = markerSize / 2.0;

    // Markörens hörn i dess egna 3D-koordinatsystem.
    // Centrum ligger i (0, 0, 0) och markören är platt.
    // Bildhörnen måste motsvara dessa punkter i samma ordning.
    std::vector<cv::Point3f> objectPoints = {
        cv::Point3f(-halfSize, halfSize, 0.0),
        cv::Point3f(halfSize, halfSize, 0.0),
        cv::Point3f(halfSize, -halfSize, 0.0),
        cv::Point3f(-halfSize, -halfSize, 0.0)};

    // solvePnP behöver beräkna rotation internt.
    // D omvandlar den inte till grader och returnerar den inte.
    cv::Vec3d rotationVector;

    // Markörens centrum relativt kameran.
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

    // Positionen får samma längdenhet som markerSize: meter.
    Pose pose;
    pose.position = translationVector;

    return pose;
}

// Beräknar det raka 3D-avståndet till markörens centrum i meter.
double PoseEstimator::distanceToMarker(const Pose &pose) const
{
    double x = pose.position[0];
    double y = pose.position[1];
    double z = pose.position[2];

    return std::sqrt(x * x + y * y + z * z);
}