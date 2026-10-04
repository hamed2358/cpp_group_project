#include <iostream>
#include <opencv2/opencv.hpp>
#include "CameraInput.hpp"
#include "MarkerDetector.hpp"
#include "MarkerRecognizer.hpp"

int main()

{

    // Open the supplied video file.

    CameraInput camera;
    MarkerDetector detector;
    MarkerRecognizer Recognizer;

    if (!camera.openCamera(0)) {

        std::cerr << "Could not open the video.\n";

        return 1;

    }

    cv::Mat frame;

    // Read and display one frame at a time.

    while (camera.readFrame(frame)) {

        cv::Mat processedFrame = detector.processFrame(frame);
        if (detector.corners.size() == 4)
         {
            double outAngle = 0.0;
            cv::Mat binaryMarker;
            if (Recognizer.processMarker(frame, detector.corners, outAngle, binaryMarker)) {
            std::cout << "Detected Angle: " << outAngle << "\n";
            }
            // std::cout << "Marker candidate found: 4 corners\n";
         }
        cv::imshow("MarkerDetector debug",  processedFrame);

        // Wait briefly; Escape closes the program.

        if (cv::waitKey(20) == 27) {

            break;

        }

    }

    return 0;

}
