#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

struct MarkerData {
    bool isDetected = false;
    int id = -1;
    int outAngle = 0; 
    std::vector<cv::Point2f> corners;
    cv::Mat binaryMarker;
};

class MarkerRecognizer{
    public:
MarkerData processMarker(const cv::Mat& frame, const std::vector<cv::Point>& corners);
};
