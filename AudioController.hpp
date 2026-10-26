#pragma once

#include "AudioControlInput.hpp"
#include <SFML/Audio.hpp>

class AudioController
{
public:
    // Assignment 2: describe why playback is stopped.
    enum class StopReason
    {
        NotStarted,
        None,
        MarkerMissing,
        InvalidDistance,
        InvalidRotation
    };

    AudioController();

    void update(const AudioControlInput &input);
    void printDebugState() const;

    // Assignment 2: expose the stop reason for diagnostics and tests.
    StopReason getStopReason() const;

private:
    sf::SoundBuffer buffer_;
    sf::Sound sound_;

    // Assignment 2: no input has been processed yet.
    StopReason stopReason_ = StopReason::NotStarted;
};