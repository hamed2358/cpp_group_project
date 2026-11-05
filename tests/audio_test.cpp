#include "AudioController.hpp"

#include <chrono>
#include <iostream>
#include <limits>
#include <thread>
#include <cassert>
#include <cmath>

// Skickar ett testfall till samma ljudobjekt.
// Varje test pågår i två sekunder.
void runCase(AudioController &audio,
             const char *description,
             bool visible,
             float distance,
             float rotation)
{
        AudioControlInput input;
        input.visible = visible;
        input.distanceMeters = distance;
        input.rotationDegrees = rotation;

        std::cout << description << std::endl;
        audio.update(input);
        audio.printDebugState();

        std::this_thread::sleep_for(std::chrono::seconds(2));
}

int main()
{
        AudioController audio;

        const float nan = std::numeric_limits<float>::quiet_NaN();
        const float infinity = std::numeric_limits<float>::infinity();

        // Kontrollera vinkelomvandlingen utan kamera eller markör.
        assert(normalizeRotationDegrees(0.0f) == 0.0f);
        assert(normalizeRotationDegrees(90.0f) == 90.0f);
        assert(normalizeRotationDegrees(180.0f) == -180.0f);
        assert(normalizeRotationDegrees(270.0f) == -90.0f);
        assert(normalizeRotationDegrees(359.0f) == -1.0f);
        assert(normalizeRotationDegrees(360.0f) == 0.0f);
        assert(normalizeRotationDegrees(-181.0f) == 179.0f);
        assert(std::isnan(normalizeRotationDegrees(nan)));
        assert(std::isinf(normalizeRotationDegrees(infinity)));

        std::cout << "Rotation normalization checks passed." << std::endl;

        // Avstånd styr tonhöjden. Rotation och volym hålls konstanta.
        runCase(audio, "1. Nara: ljus ton.", true, 0.2f, 0.0f);
        runCase(audio, "2. Langt bort: mork ton.", true, 1.0f, 0.0f);
        runCase(audio, "3. Under avstandsgransen: samma ton som test 1.",
                true, 0.1f, 0.0f);
        runCase(audio, "4. Over avstandsgransen: samma ton som test 2.",
                true, 2.0f, 0.0f);

        // Rotation styr volymen. Avstånd och tonhöjd hålls konstanta.
        runCase(audio, "5. Rotation -90: tyst.", true, 0.5f, -90.0f);
        runCase(audio, "6. Rotation 0: volym 20.", true, 0.5f, 0.0f);
        runCase(audio, "7. Rotation +90: volym 40.", true, 0.5f, 90.0f);
        runCase(audio, "8. Rotation +120: samma volym som test 7.",
                true, 0.5f, 120.0f);
        runCase(audio, "9. Rotation -120: tyst.", true, 0.5f, -120.0f);

        // Frånvaro och återstart.
        runCase(audio, "10. Giltiga varden igen: ljud.",
                true, 0.5f, 0.0f);
        runCase(audio, "11. Markoren saknas: tyst.",
                false, 0.5f, 0.0f);
        runCase(audio, "12. Markoren kommer tillbaka: ljud.",
                true, 0.5f, 0.0f);

        // Ogiltiga avstånd ska stoppa ljudet.
        runCase(audio, "13. Avstand 0: tyst.", true, 0.0f, 0.0f);
        runCase(audio, "14. Negativt avstand: tyst.", true, -0.5f, 0.0f);
        runCase(audio, "15. Avstand NaN: tyst.", true, nan, 0.0f);
        runCase(audio, "16. Oandligt avstand: tyst.",
                true, infinity, 0.0f);

        // Kontrollera återhämtning och ogiltig rotation.
        runCase(audio, "17. Giltiga varden efter felen: ljud.",
                true, 0.5f, 0.0f);
        runCase(audio, "18. Rotation NaN: tyst.", true, 0.5f, nan);
        runCase(audio, "19. Oandlig rotation: tyst.",
                true, 0.5f, infinity);
        runCase(audio, "20. Giltiga varden igen: ljud.",
                true, 0.5f, 0.0f);

        AudioControlInput stoppedInput;
        stoppedInput.visible = false;
        audio.update(stoppedInput);

        std::cout << "Testsekvens avslutad. Kontrollera utfallet manuellt."
                  << std::endl;

        return 0;
}