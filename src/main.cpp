#include <iostream>
#include <string>
#include <cmath>
#include <exception>

#include <opencv2/opencv.hpp>

#include "CameraInput.hpp"
#include "CameraCalibration.hpp"
#include "MarkerDetector.hpp"
#include "MarkerRecognizer.hpp"
#include "PoseEstimator.hpp"
#include "AudioController.hpp"

int main()
{
   // Initialize the components
    try
    {
        const double markerSize = 0.043;

        // Initialize the components.
        CameraCalibration calibration;
        CameraInput camera;
        MarkerDetector detector;
        MarkerRecognizer recognizer;

        PoseEstimator poseEstimator(
            calibration.getCameraMatrix(),
            calibration.getDistortionCoefficients());
         // Open the camera
        if (!camera.openCamera(0))
        {
            std::cerr << "Could not open camera.\n";
            return 1;
        }

        AudioController audio;
        cv::Mat frame;
        bool markerWasVisible = false;
        // Read and process camera frames
        while (camera.readFrame(frame))
        {
            // Start with no visible marker
            AudioControlInput input;

            cv::Mat processedFrame = detector.processFrame(frame);

            if (detector.corners.size() == 4)
            {
                try
                {
                    MarkerData marker = recognizer.processMarker(
                        frame, detector.corners);

                    if (marker.isDetected &&
                        marker.corners.size() == 4)
                    {
                        // Calculate the marker position
                        const auto pose = poseEstimator.estimatePoseFromScaledCorners(
                            marker.corners, markerSize);

                        if (pose)
                        {
                            // Calculate the distance to the marker
                            input.distanceMeters =
                                static_cast<float>(
                                    poseEstimator.distanceToMarker(*pose));

                             // Get the marker rotation for volume control
                            input.rotationDegrees =
                                normalizeRotationDegrees(
                                    static_cast<float>(marker.outAngle));

                            input.visible = true;
                             
                            std::cout << "Vinkel: " << input.rotationDegrees << "°\n";
                            markerWasVisible = true;
                        }
                    }
                }
                catch (const cv::Exception &error)
                {
                     // Keep the marker invisible if processing fails
                    std::cerr << "Marker/pose processing failed: "
                              << error.what() << '\n';
                }
            }

            // Update the sound based on marker data
            audio.update(input);
            if (!input.visible && markerWasVisible)
            {
                std::cout << "No marker detected.\n";
                markerWasVisible = false;
            }
               // Show the camera frame
            cv::imshow("MarkerDetector debug", processedFrame);

            // Press Esc to exit
            if (cv::waitKey(20) == 27)
            {
                break;
            }
        }

        audio.update(AudioControlInput{});
        cv::destroyAllWindows();

        return 0;
    }
    catch (const std::exception &error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}