#include "CameraInput.hpp"


//open the camera so the program can capture live videos frames:
//These frames can then used by the marker detection components. 
bool CameraInput::openCamera(int cameraIndex)
{
    return camera.open(cameraIndex);

}

//reads the next frams from the opened camera,
//The frame is stored inte Frame and can be proccessd by the other compenents. 
bool CameraInput::readFrame(cv::Mat& frame)
{
    return camera.read(frame);
}