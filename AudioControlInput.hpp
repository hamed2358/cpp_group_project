#pragma once
#include <cmath>
struct AudioControlInput
{
    // true när rätt markör och giltiga styrvärden finns.
    // Annars false, så att ljudet stoppas.
    bool visible = false;

    // Avstånd från kameran till markörens centrum, i meter.
    // Ska vara ändligt och större än 0.
    float distanceMeters = 0.0f;

    // Rattvinkel i grader: -180 <= vinkel < 180.
    // 0 = markören upprätt.
    // Positivt = medurs i kamerans ospeglade bild.
    // Negativt = moturs.
    // Ljuddelen använder -90 till +90 för volymstyrningen.
    float rotationDegrees = 0.0f;
};
inline float normalizeRotationDegrees(float degrees)
{
    if (!std::isfinite(degrees))
    {
        return degrees;
    }

    float angle = std::fmod(degrees, 360.0f);

    if (angle >= 180.0f)
    {
        angle -= 360.0f;
    }
    else if (angle < -180.0f)
    {
        angle += 360.0f;
    }

    return angle;
}