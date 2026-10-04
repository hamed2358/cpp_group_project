#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
//#include <opencv2/geometry.hpp>

struct MarkerData {
    bool isVisible = false;
    int id = -1;
    int orientation = 0; // degrees
    std::vector<cv::Point> corners;
};

class MarkerRecognizer{
    public:
//input camera frame and 4 corners
bool processMarker(const cv::Mat& frame, const std::vector<cv::Point>& corners, double& outAngle, cv::Mat& binaryMarker);
};
