#pragma once

#include "AudioControlInput.hpp"
#include <SFML/Audio.hpp>


class AudioController
{
public:
    AudioController();

    void update(const AudioControlInput& input);
    void printDebugState() const;

private:
    sf::SoundBuffer buffer_;
    sf::Sound sound_;
};