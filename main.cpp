#include <iostream>
#include <opencv2/opencv.hpp>
#include "CameraInput.hpp"

int main()

{

    // Open the supplied video file.

    CameraInput camera;

    if (!camera.openCamera(0)) {

        std::cerr << "Could not open the video.\n";

        return 1;

    }

    cv::Mat frame;

    // Read and display one frame at a time.

    while (camera.readFrame(frame)) {

        cv::imshow("OpenCV video test", frame);

        // Wait briefly; Escape closes the program.

        if (cv::waitKey(20) == 27) {

            break;

        }

    }

    return 0;

}