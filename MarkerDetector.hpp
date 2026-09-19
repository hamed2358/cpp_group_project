#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/geometry.hpp>

class MarkerDetector
{
    public:
        cv::Mat processFrame(const cv::Mat& frame);
        std::vector<cv::Point> corners;
};