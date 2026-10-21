#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <exception>

#include <opencv2/opencv.hpp>

#include "CameraInput.hpp"
#include "MarkerDetector.hpp"
#include "MarkerRecognizer.hpp"
#include "PoseEstimator.hpp"
#include "AudioController.hpp"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0]
                  << " <calibration_file> <marker_size_m>\n";
        return 1;
    }

    try
    {
        // Läs markörens verkliga sidlängd i meter.
        const std::string sizeText = argv[2];
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

        // Läs befintliga kalibreringsvärden från YAML-filen.
        cv::FileStorage calibration(argv[1], cv::FileStorage::READ);

        if (!calibration.isOpened())
        {
            std::cerr << "Could not open calibration file: "
                      << argv[1] << '\n';
            return 1;
        }

        cv::Mat cameraMatrix;
        cv::Mat distortionCoefficients;

        calibration["cameraMatrix"] >> cameraMatrix;
        calibration["distortionCoefficients"] >> distortionCoefficients;

        // Kontrollera att filen innehåller användbara matriser.
        const std::size_t coefficientCount =
            distortionCoefficients.total();

        const bool validCoefficientCount =
            coefficientCount == 4 ||
            coefficientCount == 5 ||
            coefficientCount == 8 ||
            coefficientCount == 12 ||
            coefficientCount == 14;

        if (cameraMatrix.rows != 3 ||
            cameraMatrix.cols != 3 ||
            cameraMatrix.channels() != 1 ||
            distortionCoefficients.channels() != 1 ||
            !(distortionCoefficients.rows == 1 ||
              distortionCoefficients.cols == 1) ||
            !validCoefficientCount)
        {
            std::cerr << "Invalid calibration matrix dimensions.\n";
            return 1;
        }

        cameraMatrix.convertTo(cameraMatrix, CV_64F);
        distortionCoefficients.convertTo(
            distortionCoefficients, CV_64F);

        if (!cv::checkRange(cameraMatrix) ||
            !cv::checkRange(distortionCoefficients) ||
            cameraMatrix.at<double>(0, 0) <= 0.0 ||
            cameraMatrix.at<double>(1, 1) <= 0.0)
        {
            std::cerr << "Invalid calibration values.\n";
            return 1;
        }

        // Skapa varje komponent en gång.
        CameraInput camera;
        MarkerDetector detector;
        MarkerRecognizer recognizer;
        PoseEstimator poseEstimator(
            cameraMatrix, distortionCoefficients);

        if (!camera.openCamera(0))
        {
            std::cerr << "Could not open camera.\n";
            return 1;
        }

        AudioController audio;
        cv::Mat frame;
        bool firstFrame = true;

        while (camera.readFrame(frame))
        {
            if (firstFrame)
            {
                std::cout << "Camera image size: "
                          << frame.cols << " x "
                          << frame.rows << '\n';
                firstFrame = false;
            }

            // Grundläget är ingen giltig markör: ljudet ska stoppas.
            AudioControlInput input;

            cv::Mat processedFrame = detector.processFrame(frame);

            if (detector.corners.size() == 4)
            {
                try
                {
                    MarkerData marker = recognizer.processMarker(
                        frame, detector.corners);
                    std::cout << "[Tracking] recognized=" << marker.isDetected
                              << ", corners=" << marker.corners.size()
                              << '\n';

                    if (marker.isDetected &&
                        marker.corners.size() == 4)
                    {
                        // Recognizer har förstorat hörnen med 1,5.
                        // Återställ dem för den ursprungliga fyrkantens mått.
                        std::vector<cv::Point2f> poseCorners =
                            marker.corners;

                        const cv::Point2f center =
                            (poseCorners[0] + poseCorners[1] +
                             poseCorners[2] + poseCorners[3]) *
                            0.25f;

                        for (auto &corner : poseCorners)
                        {
                            corner =
                                center + (corner - center) / 1.5f;
                        }

                        // Använd Abbes befintliga poseberäkning.
                        const auto pose = poseEstimator.estimatePose(
                            poseCorners, markerSize);

                        if (pose)
                        {
                            // Använd Abbes befintliga avståndsberäkning.
                            input.distanceMeters =
                                static_cast<float>(
                                    poseEstimator.distanceToMarker(*pose));

                            // Använd Recognizers vinkel för volymen.
                            input.rotationDegrees =
                                normalizeRotationDegrees(
                                    static_cast<float>(marker.outAngle));

                            input.visible = true;

                            std::cout
                                << "Distance (m): "
                                << input.distanceMeters
                                << ", rotation (degrees): "
                                << input.rotationDegrees
                                << '\n';
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
            audio.printDebugState();

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