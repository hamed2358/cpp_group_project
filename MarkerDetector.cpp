#include "MarkerDetector.hpp"



//proccess a camera frame to find possible square marker candidate.
//The frame is converted to grayscalea and thresholded before seraching for contours. 
cv::Mat MarkerDetector::processFrame(const cv::Mat &frame) 
{
    corners.clear();
    cv::Mat gray;
    cv::Mat binary;
    std::vector<std::vector<cv::Point>> contours;
    //create a copy of the frame for drawning the result
    cv::Mat debug = frame.clone();
    //convert the frame in the grayscale  to a black and white image. 
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, binary, 100, 255, cv::THRESH_BINARY);

    cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    // Check each contour to find a possible square candidate. 
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