#include "AudioController.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <iostream>

AudioController::AudioController()
    : sound_(buffer_)
{
    const unsigned int sampleRate = 44100;
    const double frequency = 440.0;
    const double pi = 3.141592653589793;

    // Skapa en sekund av en jämn ton.
    std::vector<std::int16_t> samples(sampleRate);

    for (unsigned int i = 0; i < sampleRate; ++i)
    {
        double time = static_cast<double>(i) / sampleRate;

        samples[i] = static_cast<std::int16_t>(
            3000.0 * std::sin(2.0 * pi * frequency * time));
    }

    // Lagra tonen som monoljud.
    if (!buffer_.loadFromSamples(
            samples.data(),
            samples.size(),
            1,
            sampleRate,
            {sf::SoundChannel::Mono}))
    {
        throw std::runtime_error("Could not create tone.");
    }

    sound_.setBuffer(buffer_);
    sound_.setLooping(true);
    sound_.setVolume(20.0f);
}

void AudioController::update(const AudioControlInput &input)
{
    // Original validation, temporarily commented out for comparison.
    /*
    // Stoppa ljudet om markören saknas eller styrvärdena är ogiltiga.
    if (!input.visible ||
        !std::isfinite(input.distanceMeters) ||
        input.distanceMeters <= 0.0f ||
        !std::isfinite(input.rotationDegrees))
    {
        sound_.stop();
        return;
    }
        */

    // Assignment 2: identify why playback must stop.
    // If multiple errors occur, prioritize a missing marker,
    // then invalid distance, then invalid rotation.
    if (!input.visible)
    {
        stopReason_ = StopReason::MarkerMissing;
    }
    else if (!std::isfinite(input.distanceMeters) ||
             input.distanceMeters <= 0.0f)
    {
        stopReason_ = StopReason::InvalidDistance;
    }
    else if (!std::isfinite(input.rotationDegrees))
    {
        stopReason_ = StopReason::InvalidRotation;
    }
    else
    {
        // Clear the previous stop reason when valid input returns.
        stopReason_ = StopReason::None;
    }

    // Preserve the original behavior: invalid input stops playback.
    if (stopReason_ != StopReason::None)
    {
        sound_.stop();
        return;
    }

    // Avstånd styr tonhöjden.
    const float nearDistance = 0.2f;
    const float farDistance = 1.0f;
    const float highPitch = 2.0f;
    const float lowPitch = 0.5f;

    // Begränsa avståndet till arbetsområdet.
    float distance = std::clamp(
        input.distanceMeters, nearDistance, farDistance);

    // Nära blir 0, långt bort blir 1.
    float distanceProportion =
        (distance - nearDistance) / (farDistance - nearDistance);

    // Nära ger ljus ton, långt bort ger mörk ton.
    float pitch =
        highPitch + distanceProportion * (lowPitch - highPitch);

    sound_.setPitch(pitch);

    // Rattvinkeln styr volymen.
    const float minRotation = -90.0f;
    const float maxRotation = 90.0f;
    const float minVolume = 0.0f;
    const float maxVolume = 40.0f;

    // Begränsa rotationen till arbetsområdet.
    float rotation = std::clamp(
        input.rotationDegrees, minRotation, maxRotation);

    // -90 grader blir 0, +90 grader blir 1.
    float rotationProportion =
        (rotation - minRotation) / (maxRotation - minRotation);

    float volume =
        minVolume + rotationProportion * (maxVolume - minVolume);

    sound_.setVolume(volume);

    // Starta bara om ljudet inte redan spelas.
    if (sound_.getStatus() != sf::SoundSource::Status::Playing)
    {
        sound_.play();
    }
}

// Assignment 2: expose the stop reason for diagnostics and tests.
AudioController::StopReason AudioController::getStopReason() const
{
    return stopReason_;
}

void AudioController::printDebugState() const
{
    const auto status = sound_.getStatus();
    const char *statusText = "Stopped";

    if (status == sf::SoundSource::Status::Playing)
    {
        statusText = "Playing";
    }
    else if (status == sf::SoundSource::Status::Paused)
    {
        statusText = "Paused";
    }

    // Assignment 2: convert the stop reason into readable text.
    const char *reasonText = "Unknown";

    switch (stopReason_)
    {
    case StopReason::NotStarted:
        reasonText = "Not started";
        break;

    case StopReason::None:
        reasonText = "None";
        break;

    case StopReason::MarkerMissing:
        reasonText = "Marker missing";
        break;

    case StopReason::InvalidDistance:
        reasonText = "Invalid distance";
        break;

    case StopReason::InvalidRotation:
        reasonText = "Invalid rotation";
        break;
    }

    std::cout << "[Audio] status=" << statusText
              << ", reason=" << reasonText // Assignment 2: added stop reason.
              << ", pitch=" << sound_.getPitch()
              << ", volume=" << sound_.getVolume()
              << std::endl;
}