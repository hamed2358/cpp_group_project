/*
Component: MarkerRecognizer
Primary owner: Jonathan
Responsibility: Recognise the marker and determine its identity and orientation.
Main contributions: Marker normalisation, pattern matching and orientation detection.
*/

#include "MarkerRecognizer.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/geometry/2d.hpp>
#include <cmath>
#include <iostream>

MarkerData MarkerRecognizer::processMarker(const cv::Mat& frame, const std::vector<cv::Point>& corners) {
    MarkerData data;
// finding the 4 different corners 
    if (corners.size() < 4) return data;
    int tlIdx = 0, brIdx = 0, trIdx = -1, blIdx = -1;
    double minSum = corners[0].x + corners[0].y; 
    double maxSum = minSum;

    for (int i = 0; i < 4; ++i) {
        double sum = corners[i].x + corners[i].y; 
        if (sum < minSum) {
            minSum = sum;
            tlIdx = i;
        }
        if (sum > maxSum) {
            maxSum = sum;
            brIdx = i;
        }
    }

    for (int i = 0; i < 4; ++i) {
        if (i == tlIdx || i == brIdx) continue;
        if (corners[i].y - corners[i].x < 0) {
            trIdx = i;
        } else {
            blIdx = i;
        }
    }
    if (trIdx == -1 || blIdx == -1) {
        return data;
    }

    std::vector<cv::Point2f> srcPoints = {
        corners[tlIdx],
        corners[trIdx],
        corners[brIdx],
        corners[blIdx]
    };

// scale outwards (adds buffer around the marker)
    cv::Point2f center = (srcPoints[0] + srcPoints[1] + srcPoints[2] + srcPoints[3]) * 0.25f;
    float scaleFactor = 1.5f; 
    for (auto& pt : srcPoints) {
        pt = center + (pt - center) * scaleFactor;
    }

    std::vector<cv::Point2f> dstPoints = {
    cv::Point2f(0, 0),
    cv::Point2f(200, 0),
    cv::Point2f(200, 200),
    cv::Point2f(0, 200)
    };
    data.corners = srcPoints;
// Warp perspective to straighten it out 
    cv::Mat transformMatrix = cv::getPerspectiveTransform(srcPoints, dstPoints);
    cv::Mat warpedMarker;
    cv::warpPerspective(frame, warpedMarker, transformMatrix, cv::Size(200, 200));

// debug
    cv::imshow("Warped Marker", warpedMarker);
    cv::waitKey(1); 

// calcualte top edge tilt
    double deltaX = srcPoints[1].x - srcPoints[0].x;
    double deltaY = srcPoints[1].y - srcPoints[0].y;
    data.outAngle = std::atan2(deltaY, deltaX) * (180.00 / CV_PI);
    data.outAngle = std::fmod(data.outAngle + 360.0, 360.0);
  double edgeTilt = 90.0 - std::fmod(data.outAngle, 90.0);
   

// binarization
    cv::Mat grayMarker;
    cv::cvtColor(warpedMarker, grayMarker, cv::COLOR_BGR2GRAY);
    cv::threshold(grayMarker, data.binaryMarker, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);


    bool isDetected = false;
    int rotFlags[] = {cv::ROTATE_90_CLOCKWISE, cv::ROTATE_180, cv::ROTATE_90_COUNTERCLOCKWISE};
    cv::Mat currentRotation;

// cell 1/2 for pattern detection
    double c1 = 1.6, r1 = 1;
    double c2 = 2.6, r2 = 2;

    cv::Rect cell1Rect(40 + c1 * 30, 40 + r1 * 30, 30, 30);
    cv::Rect cell2Rect(40 + c2 * 30, 40 + r2 * 30, 30, 30);
    int markerAngles[] = {0, 90, 180, 270};
// check marker start rotation and if it is our marker
    int bestRotationIdx = 0;
    double minBrightnessSum = 1e9;
    cv::Mat bestRotationMatch = data.binaryMarker;
    bool foundValid = false;

        for (int i = 0; i < 4; ++i) {
            cv::Mat currentRotation;
            if (i == 0) {
                currentRotation = data.binaryMarker;
            } else {
                cv::rotate(data.binaryMarker, currentRotation, rotFlags[i - 1]);
            }

            double brightness1 = cv::mean(currentRotation(cell1Rect))[0];
            double brightness2 = cv::mean(currentRotation(cell2Rect))[0];
            std::cout << "rotation: " << i << "brightness1: " << brightness1
          << " brightness2: " << brightness2 << std::endl;

          
            
            if (brightness1 < 140 && brightness2 < 140) {
                double totalBrightness = brightness1 + brightness2;
                
                if (totalBrightness < minBrightnessSum) {
                    minBrightnessSum = totalBrightness;
                    bestRotationIdx = i;
                    bestRotationMatch = currentRotation;
                    foundValid = true;
                }
            }
        }
    // if our marker then add the values
    if (foundValid) {
        std::cout << "bestRotationIdx: " << bestRotationIdx << std::endl;

        std::cout << "rawAngle: " << data.outAngle
          << " edgeTilt: " << edgeTilt
          << " markerAngle: " << markerAngles[bestRotationIdx] << std::endl;
    
        
        data.outAngle = markerAngles[bestRotationIdx] + edgeTilt ;
        
        if (edgeTilt > 44) {
            if (markerAngles[bestRotationIdx] == 0) {
                data.outAngle += 270; 
            } else {
                data.outAngle -= 90; 
            }
        }
        data.outAngle = std::fmod(data.outAngle + 360.0, 360.0);
        data.binaryMarker = bestRotationMatch;
        data.isDetected = true;
        data.id = -1; 
    } 
    // debug
    cv::Mat cellDebugImg;
    cv::cvtColor(data.binaryMarker, cellDebugImg, cv::COLOR_GRAY2BGR);
    cv::rectangle(cellDebugImg, cell1Rect, cv::Scalar(0, 0, 255), 2); 
    cv::rectangle(cellDebugImg, cell2Rect, cv::Scalar(0, 255, 0), 2); 
    cv::imshow("Cell Detection Debug", cellDebugImg);
    cv::waitKey(30);

// Debug
/*
if (data.isDetected) {
    std::string debug = "test_case_angle_" + std::to_string((int)data.outAngle) + ".png";
    cv::imwrite(debug, cellDebugImg);
}
*/

return data; 
}
