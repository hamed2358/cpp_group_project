/*
Component: CameraCalibration
Primary owner: Abdulmajid
Responsibility: Store and provide the camera calibration parameters.
Main contributions: Camera matrix and distortion coefficients used for 3D position estimation.
*/
#pragma once

#include <opencv2/core.hpp>

// Stores the camera calibration parameters.
class CameraCalibration
{
public:
    // Initializes the camera matrix and distortion coefficients.
    CameraCalibration();

    // Returns the camera's intrinsic matrix.
    const cv::Mat &getCameraMatrix() const;

    // Returns the lens distortion coefficients.
    const cv::Mat &getDistortionCoefficients() const;

private:
    // Camera intrinsic parameters.
    cv::Mat cameraMatrix;

    // Lens distortion parameters.
    cv::Mat distortionCoefficients;
};