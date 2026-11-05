#include "AudioController.hpp"

#include <exception>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

int main()
{
    try
    {
        using StopReason = AudioController::StopReason;

        AudioController audio;

        int checks = 0;
        int failures = 0;

        // Check the public stop reason and diagnostic output.
        auto checkState = [&](const std::string &name,
                              StopReason expectedReason,
                              const std::string &expectedText,
                              bool expectedPlaying)
        {
            ++checks;

            std::ostringstream captured;
            auto *originalBuffer = std::cout.rdbuf(captured.rdbuf());

            audio.printDebugState();

            std::cout.rdbuf(originalBuffer);

            const std::string output = captured.str();

            const std::string expectedStatus =
                expectedPlaying
                    ? "status=Playing,"
                    : "status=Stopped,";

            const bool reasonMatches =
                audio.getStopReason() == expectedReason;

            const bool textMatches =
                output.find(", reason=" + expectedText + ",") !=
                std::string::npos;

            const bool statusMatches =
                output.find(expectedStatus) != std::string::npos;

            const bool passed =
                reasonMatches && textMatches && statusMatches;

            std::cout
                << (passed ? "[PASS] " : "[FAIL] ")
                << name << '\n';

            if (!passed)
            {
                ++failures;

                std::cout
                    << "  Expected reason: " << expectedText << '\n'
                    << "  Expected status: "
                    << (expectedPlaying ? "Playing" : "Stopped")
                    << '\n'
                    << "  Getter matches: "
                    << (reasonMatches ? "yes" : "no") << '\n'
                    << "  Actual output: " << output;
            }
        };

        // Before the first update, playback has not started.
        checkState(
            "Initial state",
            StopReason::NotStarted,
            "Not started",
            false);

        const AudioControlInput validInput{
            true, 0.5f, 0.0f};

        audio.update(validInput);

        checkState(
            "Valid input starts playback",
            StopReason::None,
            "None",
            true);

        const float nan =
            std::numeric_limits<float>::quiet_NaN();

        const float infinity =
            std::numeric_limits<float>::infinity();

        struct TestCase
        {
            const char *name;
            AudioControlInput input;
            StopReason expectedReason;
            const char *expectedText;
        };

        const std::vector<TestCase> cases = {
            {"Missing marker",
             {false, 0.5f, 0.0f},
             StopReason::MarkerMissing,
             "Marker missing"},
            {"Zero distance",
             {true, 0.0f, 0.0f},
             StopReason::InvalidDistance,
             "Invalid distance"},
            {"Negative distance",
             {true, -0.5f, 0.0f},
             StopReason::InvalidDistance,
             "Invalid distance"},
            {"NaN distance",
             {true, nan, 0.0f},
             StopReason::InvalidDistance,
             "Invalid distance"},
            {"Positive infinite distance",
             {true, infinity, 0.0f},
             StopReason::InvalidDistance,
             "Invalid distance"},
            {"Negative infinite distance",
             {true, -infinity, 0.0f},
             StopReason::InvalidDistance,
             "Invalid distance"},
            {"NaN rotation",
             {true, 0.5f, nan},
             StopReason::InvalidRotation,
             "Invalid rotation"},
            {"Positive infinite rotation",
             {true, 0.5f, infinity},
             StopReason::InvalidRotation,
             "Invalid rotation"},
            {"Negative infinite rotation",
             {true, 0.5f, -infinity},
             StopReason::InvalidRotation,
             "Invalid rotation"},
            {"Missing marker takes priority over invalid values",
             {false, nan, nan},
             StopReason::MarkerMissing,
             "Marker missing"},
            {"Invalid distance takes priority over invalid rotation",
             {true, 0.0f, nan},
             StopReason::InvalidDistance,
             "Invalid distance"}};

        for (const auto &test : cases)
        {
            // Each failure is tested after valid playback.
            audio.update(test.input);

            checkState(
                test.name,
                test.expectedReason,
                test.expectedText,
                false);

            // Valid input must clear the reason and restart playback.
            audio.update(validInput);

            checkState(
                std::string("Recovery after: ") + test.name,
                StopReason::None,
                "None",
                true);
        }

        // Zero volume is valid input, not a validation failure.
        audio.update(AudioControlInput{true, 0.5f, -90.0f});

        checkState(
            "Zero volume does not create a stop reason",
            StopReason::None,
            "None",
            true);

        // Stop playback before the test exits.
        audio.update(AudioControlInput{});

        std::cout
            << "\nChecks: " << checks
            << "\nFailures: " << failures << '\n';

        return failures == 0 ? 0 : 1;
    }
    catch (const std::exception &error)
    {
        std::cerr << "Test error: " << error.what() << '\n';
        return 1;
    }
}