# Assignment 2 – AudioController Improvements

Student: Abdulmajid Almohamed
Group: TS
Branch: assignment2/audio-stop-reasons
Original version: a80bc20

## What I investigated

I reviewed component E, AudioController, and tested Ali’s original
version with 20 test cases. The reported playback status, pitch and
volume matched the expected results.

However, when playback stopped, the debug output only showed `Stopped`.
It did not explain whether the marker was missing, the distance was
invalid or the rotation was invalid.

## What I changed

I added stop reasons and a `getStopReason()` function. The debug output
now shows `Marker missing`, `Invalid distance` or `Invalid rotation`.
When valid input returns, the reason changes to `None`.

If several errors occur together, a missing marker takes priority,
followed by invalid distance and invalid rotation. The original
validation is kept as commented code for comparison.

## How I tested it

I ran the same 20 cases again. The reported status, pitch and volume
were unchanged, while the output now explained each stop.

I also added an automated test covering the initial state, invalid
values, error priority and recovery. It checks both the getter and
debug output.

Result: **25 checks, 0 failures.**

Evidence:

- [Before](assignment2-audio-stop-reasons/before-output.txt)
- [After](assignment2-audio-stop-reasons/after-output.txt)
- [Automated test](assignment2-audio-stop-reasons/stop-reasons-output.txt)

## Build compatibility

The full project initially failed to build with OpenCV 5 because
geometry declarations were missing.

I added a version check in CMakeLists.txt that includes the geometry
header for OpenCV 5. I did not change the A–C source files.

After the change, both `camera_grab` and `audio_test` built successfully
with OpenCV 5.0.0 and SFML 3.0.2.

## Running the checks

Build the project:

```bash
cmake -S . -B build
cmake --build build
```

Build and run the automated test on my Mac:

```bash
c++ -std=c++17 \
  test_audio_stop_reasons.cpp AudioController.cpp \
  -I/opt/homebrew/opt/sfml/include \
  -L/opt/homebrew/opt/sfml/lib \
  -Wl,-rpath,/opt/homebrew/opt/sfml/lib \
  -lsfml-audio -lsfml-system \
  -o build/test_audio_stop_reasons

./build/test_audio_stop_reasons
```

