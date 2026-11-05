/*
Component: MarkerRecognizer
Primary owner: Jonathan
Responsibility: Recognise the marker and determine its identity and orientation.
Main contributions: Marker normalisation, pattern matching and orientation detection.
*/

#include "MarkerRecognizer.hpp"
#include <opencv2/opencv.hpp>
// #include <opencv2/geometry/2d.hpp>
#include <cmath>
#include <iostream>

MarkerData MarkerRecognizer::processMarker(const cv::Mat &frame, const std::vector<cv::Point> &corners)
{
    MarkerData data;
    // finding the 4 different corners
    if (corners.size() < 4)
        return data;

    std::vector<cv::Point> orderedCorners;
    cv::convexHull(corners, orderedCorners, false);

    if (orderedCorners.size() != 4)
    {
        return data;
    }

    
    int startIdx = 0;

    for (int i = 1; i < 4; ++i)
    {
        if (orderedCorners[i].x + orderedCorners[i].y <
            orderedCorners[startIdx].x + orderedCorners[startIdx].y)
        {
            startIdx = i;
        }
    }

    std::vector<cv::Point2f> srcPoints;

    for (int i = 0; i < 4; ++i)
    {
        srcPoints.emplace_back(orderedCorners[(startIdx + i) % 4]);
    }

    // scale outwards (adds buffer around the marker)
    cv::Point2f center = (srcPoints[0] + srcPoints[1] + srcPoints[2] + srcPoints[3]) * 0.25f;
    float scaleFactor = 1.5f;
    for (auto &pt : srcPoints)
    {
        pt = center + (pt - center) * scaleFactor;
    }

    std::vector<cv::Point2f> dstPoints = {
        cv::Point2f(0, 0),
        cv::Point2f(200, 0),
        cv::Point2f(200, 200),
        cv::Point2f(0, 200)};
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

    double whiteC1 = 1.6, whiteR1 = 2;
    double whiteC2 = 2.6, whiteR2 = 1;

    cv::Rect cell1Rect(40 + c1 * 30, 40 + r1 * 30, 30, 30);
    cv::Rect cell2Rect(40 + c2 * 30, 40 + r2 * 30, 30, 30);

    cv::Rect whiteCell1Rect(40 + whiteC1 * 30, 40 + whiteR1 * 30, 30, 30);
    cv::Rect whiteCell2Rect(40 + whiteC2 * 30, 40 + whiteR2 * 30, 30, 30);
    int markerAngles[] = {0, 90, 180, 270};
    // check marker start rotation and if it is our marker
    int bestRotationIdx = 0;
    double minBrightnessSum = 1e9;
    cv::Mat bestRotationMatch = data.binaryMarker;
    bool foundValid = false;

    for (int i = 0; i < 4; ++i)
    {
        cv::Mat currentRotation;
        if (i == 0)
        {
            currentRotation = data.binaryMarker;
        }
        else
        {
            cv::rotate(data.binaryMarker, currentRotation, rotFlags[i - 1]);
        }

        double brightness1 = cv::mean(currentRotation(cell1Rect))[0];
        double brightness2 = cv::mean(currentRotation(cell2Rect))[0];

        double whiteBrightness1 = cv::mean(currentRotation(whiteCell1Rect))[0];
        double whiteBrightness2 = cv::mean(currentRotation(whiteCell2Rect))[0];
        

        if (brightness1 < 140 && brightness2 < 140 && whiteBrightness1 > 200 && whiteBrightness2 > 200)
        {
            double totalBrightness = brightness1 + brightness2;

            if (totalBrightness < minBrightnessSum)
            {
                minBrightnessSum = totalBrightness;
                bestRotationIdx = i;
                bestRotationMatch = currentRotation;
                foundValid = true;
            }
        }
    }
    // if our marker then add the values
    if (foundValid)
    {
        data.outAngle = markerAngles[bestRotationIdx] + edgeTilt;

        if (edgeTilt > 44)
        {
            if (markerAngles[bestRotationIdx] == 0)
            {
                data.outAngle += 270;
            }
            else
            {
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

    cv::rectangle(cellDebugImg, whiteCell1Rect, cv::Scalar(255, 0, 0), 2);
    cv::rectangle(cellDebugImg, whiteCell2Rect, cv::Scalar(255, 255, 0), 2);

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
