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
    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);
    cv::threshold(gray, binary, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    double bestScore = 0; 
    std::vector<cv::Point> bestCandidate;
    // Check each contour to find a possible square candidate. 
    for(const auto& contour : contours)
    {
        std::vector<cv::Point> approx;
        cv::approxPolyDP(contour, approx, 0.02 * cv::arcLength(contour, true), true);
        
        if(approx.size() == 4)
        {
            double area = cv::contourArea(approx);
            if(area > 1000)
            {
                cv::Rect boundingBox = cv::boundingRect(approx);
                double aspectRatio = (double)boundingBox.width / boundingBox.height;
              
                if(aspectRatio >= 0.75 && aspectRatio <= 1.25)
                {
                    if(area > bestScore)
                    {
                        bestScore = area;
                        bestCandidate = approx;
                    }
                }
            }
        }
    }
    
    if(!bestCandidate.empty())
    {
        corners = bestCandidate;
        cv::polylines(debug, corners, true, cv::Scalar(0, 255, 0), 3);
    }
    
    return debug;
}
