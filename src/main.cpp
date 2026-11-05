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

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0]
                  << " <marker_size_m>\n";
        return 1;
    }

    try
    {
        // Läs markörens verkliga sidlängd i meter.
        const std::string sizeText = argv[1];
        std::size_t parsed = 0;
        const double markerSize = std::stod(sizeText, &parsed);

        if (parsed != sizeText.size() ||
            !std::isfinite(markerSize) ||
            markerSize <= 0.0)
        {
            std::cerr
                << "Marker size must be a positive finite number in metres.\n";
            return 1;
        }

        // Initialize the components.
        CameraCalibration calibration;
        CameraInput camera;
        MarkerDetector detector;
        MarkerRecognizer recognizer;

        PoseEstimator poseEstimator(
            calibration.getCameraMatrix(),
            calibration.getDistortionCoefficients());

        if (!camera.openCamera(0))
        {
            std::cerr << "Could not open camera.\n";
            return 1;
        }

        AudioController audio;
        cv::Mat frame;

        while (camera.readFrame(frame))
        {
            // Grundläget är ingen giltig markör: ljudet ska stoppas.
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
                        // Estimate pose using the corners from MarkerRecognizer.
                        const auto pose = poseEstimator.estimatePoseFromScaledCorners(
                            marker.corners, markerSize);

                        if (pose)
                        {
                            // Använd den befintliga avståndsberäkning.
                            input.distanceMeters =
                                static_cast<float>(
                                    poseEstimator.distanceToMarker(*pose));

                            // Använd Recognizers vinkel för volymen.
                            input.rotationDegrees =
                                normalizeRotationDegrees(
                                    static_cast<float>(marker.outAngle));

                            input.visible = true;
                        }
                    }
                }
                catch (const cv::Exception &error)
                {
                    // input behåller grundläget och ljudet stoppas.
                    std::cerr << "Marker/pose processing failed: "
                              << error.what() << '\n';
                }
            }

            // AudioController sköter validering och ljudstyrning.
            audio.update(input);

            cv::imshow("MarkerDetector debug", processedFrame);

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