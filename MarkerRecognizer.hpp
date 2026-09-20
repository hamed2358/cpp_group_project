#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/geometry.hpp>

class MarkerRecognizer
{
    public:
//input camera frame and 4 corners
MarkerData recognizeMarker(const cv::Mat& frame, const std::vector<cv::Point>& corners);


//Marker?
//square to image
// compare image to known marker
//if match determine marker id 
// if not send qustion if user wants to add new marker
// give orentation

//output marker visability,marker id orientation and 4 corners
return MarkerData;
};
struct MarkerData {
    bool isVisible = false;
    int id = -1;
    int orientation = 0; // degrees
    std::vector<cv::Point> corners;
};
