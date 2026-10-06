#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
//#include <opencv2/geometry.hpp>

struct MarkerData {
    bool isDetected = false;
    int id = -1;
    double outAngle = 0; // degrees
    std::vector<cv::Point2f> corners;
    cv::Mat binaryMarker;
};

class MarkerRecognizer{
    public:
//input camera frame and 4 corners
MarkerData processMarker(const cv::Mat& frame, const std::vector<cv::Point>& corners);
};
