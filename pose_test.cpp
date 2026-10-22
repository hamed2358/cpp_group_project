#include "PoseEstimator.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

// Kör ett testfall och jämför resultatet med förväntat beteende.
bool runCase(PoseEstimator &estimator,
             const char *name,
             const std::vector<cv::Point2f> &corners,
             bool expectValid)
{
    try
    {
        const auto pose = estimator.estimatePose(corners, 0.1);

        bool passed;

        if (expectValid)
        {
            // Det giltiga fallet ska ge 0,5 m inom angiven tolerans.
            passed = pose &&
                std::abs(estimator.distanceToMarker(*pose) - 0.5)
                    < 0.0001;
        }
        else
        {
            // Ogiltiga koordinater ska avvisas med std::nullopt.
            passed = !pose;
        }

        std::cout << (passed ? "PASS: " : "FAIL: ") << name;

        if (pose)
        {
            std::cout << ", distance="
                      << estimator.distanceToMarker(*pose);
        }
        else
        {
            std::cout << ", returned nullopt";
        }

        std::cout << '\n';
        return passed;
    }
    catch (const cv::Exception &error)
    {
        // Undantag räknas som misslyckade tester.
        std::cout << "FAIL: " << name
                  << ", exception: " << error.what() << '\n';
        return false;
    }
}

int main()
{
    // Konstruerad kamerakalibrering utan linsförvrängning.
    const cv::Mat cameraMatrix =
        (cv::Mat_<double>(3, 3) <<
            800, 0, 320,
            0, 800, 240,
            0, 0, 1);

    const cv::Mat distortion =
        cv::Mat::zeros(1, 5, CV_64F);

    PoseEstimator estimator(cameraMatrix, distortion);

    // Hörnen motsvarar en markör med sidan 0,1 m på avståndet 0,5 m.
    const std::vector<cv::Point2f> validCorners = {
        {240, 160}, {400, 160},
        {400, 320}, {240, 320}
    };

    int passed = 0;

    passed += runCase(
        estimator, "Valid corners", validCorners, true);

    // Kontrollera att en NaN-koordinat avvisas.
    auto invalidCorners = validCorners;
    invalidCorners[0].x =
        std::numeric_limits<float>::quiet_NaN();

    passed += runCase(
        estimator, "NaN coordinate", invalidCorners, false);

    // Återställ hörnen och kontrollera en oändlig koordinat.
    invalidCorners = validCorners;
    invalidCorners[1].y =
        std::numeric_limits<float>::infinity();

    passed += runCase(
        estimator, "Infinite coordinate", invalidCorners, false);

    std::cout << passed << "/3 tests passed.\n";

    return passed == 3 ? 0 : 1;
}