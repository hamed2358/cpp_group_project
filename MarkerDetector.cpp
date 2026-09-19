#include "MarkerDetector.hpp"


cv::Mat MarkerDetector::processFrame(const cv::Mat &frame) 
{
    corners.clear();
    cv::Mat gray;
    cv::Mat binary;
    std::vector<std::vector<cv::Point>> contours;
    cv::Mat debug = frame.clone();
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, binary, 100, 255, cv::THRESH_BINARY);
    cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    for(const auto& contour : contours)
    {
        std::vector<cv::Point> approx;
        cv::approxPolyDP(contour, approx, 0.02 * cv::arcLength(contour, true), true);
        if(approx.size() == 4)
        {
            if(cv::contourArea(approx) > 1000)
            {
                corners = approx;
                cv::polylines(debug, approx, true, cv::Scalar(0, 255, 0), 3);
            }

        }
    }
    return debug;
}