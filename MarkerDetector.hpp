#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/geometry.hpp>


//Detects possible square marker candidates in a camera fram. 
//It proccess the frame and stores the four corners of a detected candidate.
class MarkerDetector
{
    public:
    // process  a frame and returns an image with the detected candidate mark
        cv::Mat processFrame(const cv::Mat& frame);
        //Stores the four corners of the detected candidate. 
        std::vector<cv::Point> corners;
};