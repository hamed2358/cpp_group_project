#pragma once

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