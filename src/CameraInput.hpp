#pragma once
#include <opencv2/opencv.hpp>

//Handlas the camera input for the project 
//It opens a camera and provides frames that can be used by other components. 
class CameraInput
{
    public:
    //open the selectet camera
        bool openCamera(int cameraIndex);
        //reads a frame fram the camera
        bool readFrame(cv::Mat& frame);

    private:
    //stores the camera connectione
        cv::VideoCapture camera; 
};