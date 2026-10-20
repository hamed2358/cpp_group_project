/*
Component: PoseEstimator
Primary owner: Abdulmajid
Checkpoint: [Checkpoint]
Responsibility: Estimate the marker's 3D position, rotation and distance from the camera.
Main contributions: Pose estimation using marker corners and camera calibration, position and rotation calculation, and distance calculation.
*/

#include "PoseEstimator.hpp"
#include <cmath>

// Skapar PoseEstimator och sparar kamerans kalibreringsdata.
// Dessa värden kommer senare användas för att beräkna markerns pose.
PoseEstimator::PoseEstimator(const cv::Mat &cameraMatrix,
                             const cv::Mat &distortionCoefficients)
{
    // Kopierar kameramatrisen och sparar den i objektet.
    cameraMatrix_ = cameraMatrix.clone();

    // Kopierar linsens distortionsvärden och sparar dem i objektet.
    distortionCoefficients_ = distortionCoefficients.clone();
}

// Försöker beräkna markerns 3D-position och rotation.
// Returnerar en Pose om beräkningen lyckas.
// Om informationen är felaktig eller solvePnP misslyckas returneras std::nullopt.
std::optional<Pose> PoseEstimator::estimatePose(
    const std::vector<cv::Point2f> &imageCorners,
    double markerSize) const
{
    // En fyrkantig marker måste ha exakt fyra hörn.
    if (imageCorners.size() != 4)
    {
        // Inget giltigt resultat kan beräknas.
        return std::nullopt;
    }

    // Markerens verkliga storlek måste vara större än noll.
    if (markerSize <= 0.0)
    {
        // En ogiltig storlek gör poseberäkningen omöjlig.
        return std::nullopt;
    }

    // Beräknar halva markerens verkliga storlek.
    // Vi använder detta för att placera markerens centrum i (0, 0, 0).
    double halfSize = markerSize / 2.0;

    // Beskriver markerens fyra hörn i dess egna 3D-koordinatsystem.
    // Markern är platt, därför är Z = 0 för alla hörn.
    std::vector<cv::Point3f> objectPoints = {
        cv::Point3f(-halfSize, halfSize, 0.0),
        cv::Point3f(halfSize, halfSize, 0.0),
        cv::Point3f(halfSize, -halfSize, 0.0),
        cv::Point3f(-halfSize, -halfSize, 0.0)};

    // Här sparar OpenCV den beräknade rotationen.
    cv::Vec3d rotationVector;

    // Här sparar OpenCV den beräknade positionen.
    cv::Vec3d translationVector;

    // solvePnP jämför de verkliga 3D-punkterna med punkterna i kamerabilden.
    // Funktionen använder även kamerans kalibrering.
    bool success = cv::solvePnP(
        objectPoints,
        imageCorners,
        cameraMatrix_,
        distortionCoefficients_,
        rotationVector,
        translationVector);

    // Kontrollera om OpenCV lyckades beräkna en pose.
    if (!success)
    {
        // Ingen giltig pose kunde beräknas.
        return std::nullopt;
    }

    // Omvandlar Rodrigues-vektorn till en rotationsmatris.
    cv::Mat rotationMatrix;
    cv::Rodrigues(rotationVector, rotationMatrix);

    // Beräknar markerns höger/vänster-rotation (yaw).
    double yawRadians = std::atan2(
        rotationMatrix.at<double>(0, 2),
        rotationMatrix.at<double>(2, 2));

    double yawDegrees = yawRadians * 180.0 / CV_PI;

    // Markerns neutrala läge ligger runt ±180 grader.
    // Flyttar nollpunkten så att en marker rakt mot kameran blir 0 grader.
    if (yawDegrees > 0.0)
    {
        yawDegrees -= 180.0;
    }
    else
    {
        yawDegrees += 180.0;
    }

    // Skapar resultatet.
    Pose pose;

    // Sparar markerns X-, Y- och Z-position.
    pose.position = translationVector;

    // Sparar höger/vänster-rotationen i grader.
    pose.rotation = yawDegrees;

    return pose;
}

// Beräknar det raka 3D-avståndet mellan kameran och markern.
// Positionen innehåller X, Y och Z relativt kameran.
double PoseEstimator::distanceToMarker(const Pose &pose) const
{
    // Hämtar markerns position åt vänster/höger.
    double x = pose.position[0];

    // Hämtar markerns position uppåt/nedåt.
    double y = pose.position[1];

    // Hämtar markerns position framåt från kameran.
    double z = pose.position[2];

    // Beräknar det totala 3D-avståndet med Pythagoras sats.
    double distance = std::sqrt(x * x + y * y + z * z);

    // Skickar tillbaka avståndet.
    return distance;
}