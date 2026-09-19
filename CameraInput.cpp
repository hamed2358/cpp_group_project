#include "CameraInput.hpp"

bool CameraInput::openCamera(int cameraIndex)
{
    return camera.open(cameraIndex);

}

bool CameraInput::readFrame(cv::Mat& frame)
{
    return camera.read(frame);
}