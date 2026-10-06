/*
Component: MarkerRecognizer
Primary owner: Jonathan
Checkpoint: [Checkpoint]
Responsibility: Recognise the marker and determine its identity and orientation.
Main contributions: Marker normalisation, pattern matching and orientation detection.
*/

#include "MarkerRecognizer.hpp"
#include <opencv2/opencv.hpp>
#include <cmath>

MarkerData MarkerRecognizer::processMarker(const cv::Mat& frame, const std::vector<cv::Point>& corners) {
    MarkerData data;
    if (corners.size() < 4) return data;
//Find Top-Left (min sum) and Bottom-Right (max sum)
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

//Find Bottom-Lef (-) and Top Right (+)
    for (int i = 0; i < 4; ++i) {
        if (i == tlIdx || i == brIdx) continue;
    // Check y - x: negative is Top-Right, positive is Bottom-Left
        if (corners[i].y - corners[i].x < 0) {
            trIdx = i;
        } else {
            blIdx = i;
        }
    }
    if (trIdx == -1 || blIdx == -1) {
        return data;
    }

//srcPoints in clockwise order: TL, TR, BR, BL
    std::vector<cv::Point2f> srcPoints = {
        corners[tlIdx],
        corners[trIdx],
        corners[brIdx],
        corners[blIdx]
    };

// Calculate the center of the detected inner square
    cv::Point2f center = (srcPoints[0] + srcPoints[1] + srcPoints[2] + srcPoints[3]) * 0.25f;

//  Scale each corner outward to capture the outer black rim
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

    // Visualize the normalized marker for debugging
    cv::imshow("Warped Marker", warpedMarker);
    cv::waitKey(30); // Keeps the window responsive so it doesn't freeze

// Calculate rotation angle from top edge
    double deltaX = srcPoints[1].x - srcPoints[0].x;
    double deltaY = srcPoints[1].y - srcPoints[0].y;
    data.outAngle = std::atan2(deltaY, deltaX) * (180.0 / CV_PI);

// Grayscale & Binarization
    cv::Mat grayMarker;
    cv::cvtColor(warpedMarker, grayMarker, cv::COLOR_BGR2GRAY);
    cv::threshold(grayMarker, data.binaryMarker, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
// Assume binaryMarker is the 200x200 binarized image
bool isDetected = false;
int rotFlags[] = {cv::ROTATE_90_CLOCKWISE, cv::ROTATE_180, cv::ROTATE_90_COUNTERCLOCKWISE};
cv::Mat currentMarker;
int detectedAngle = 0;

// Define the top-left square 
    cv::Rect topLeftRect(0, 0, 40, 40);
    for (int i = 0; i < 4; ++i) {
        if (i == 0) {
            currentMarker = data.binaryMarker;
        } else {
            cv::rotate(data.binaryMarker, currentMarker, rotFlags[i - 1]);
        }
    // Check corner brightness and add tilt + rotation angle from top edge  
        double brightness = cv::mean(currentMarker(topLeftRect))[0];
        int markerAngles[] = {0, 270, 90, 180};
        if (brightness > 127) {
            detectedAngle = markerAngles[i]; 
            data.outAngle = data.outAngle + detectedAngle;
            data.binaryMarker = currentMarker;
            data.isDetected = true;
            data.id = -1;
            break; 
        }
    }
// Marker successfully processed
 return data; 
}
