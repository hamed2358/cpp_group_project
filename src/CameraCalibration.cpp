/*
Component: CameraCalibration
Primary owner: Abdulmajid
Responsibility: Store and provide the camera calibration parameters.
Main contributions: Camera matrix and distortion coefficients used for 3D position estimation.
*/
#include "CameraCalibration.hpp"

CameraCalibration::CameraCalibration()
{
    // Camera intrinsic matrix obtained from calibration.
    cameraMatrix = (cv::Mat_<double>(3, 3) << 1430.881855810425, 0, 957.6017801050372,
                    0, 1422.94033131879, 539.8397434076488,
                    0, 0, 1);

    // Lens distortion coefficients obtained from calibration.
    distortionCoefficients = (cv::Mat_<double>(1, 5) << -0.02628445241735195,
                              0.8886543785990032,
                              0.00007584509304828113,
                              0.000723217282236496,
                              -2.580011066570094);
}

// Returns the camera matrix used for pose estimation.
const cv::Mat &CameraCalibration::getCameraMatrix() const
{
    return cameraMatrix;
}

// Returns the distortion coefficients used for pose estimation.
const cv::Mat &CameraCalibration::getDistortionCoefficients() const
{
    return distortionCoefficients;
}