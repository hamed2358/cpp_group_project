## Hamed Sarwari

### CameraInput

Files:

- CameraInput.hpp
- CameraInput.cpp

Responsible for opening the camera and providing camera frames to the other components.

Branch: feature/camera-input  
Pull Request: https://github.com/hamed2358/cpp_group_project/pull/4

### MarkerDetector

Files:

- MarkerDetector.hpp
- MarkerDetector.cpp

Responsible for processing camera frames and detecting possible square marker candidates.

Branch: feature/marker-detector  
Pull Request: https://github.com/hamed2358/cpp_group_project/pull/5

### Assignment 2 – MarkerRecognizer Integration

Files:

- CMakeLists.txt
- MarkerDetector.cpp
- MarkerRecognizer.cpp

Investigated and improved the integration between MarkerDetector and MarkerRecognizer using reproducible build and marker rotation tests.

Branch: assignment2/marker-recognizer-integration  
Pull Request: https://github.com/hamed2358/cpp_group_project/pull/11

## Ali Akartei

### AudioController

Files:

- AudioControlInput.hpp
- AudioController.hpp
- AudioController.cpp
- audio_test.cpp

Responsible for generating a looping tone and controlling
playback, pitch and volume using marker visibility,
distance and rotation.

Branch: feature/audio-controller  
Pull Request: https://github.com/hamed2358/cpp_group_project/pull/10


## Abdulmajid Almohamed

### PoseEstimator

Files:

- PoseEstimator.hpp
- PoseEstimator.cpp

Responsible for estimating the marker's 3D position and distance
relative to the camera using marker corners and camera calibration.

Branch: feature/pose-estimator
Pull Request: https://github.com/hamed2358/cpp_group_project/pull/7


### Assignment 2 – AudioController

Files:

- AudioController.hpp
- AudioController.cpp
- test_audio_stop_reasons.cpp

Investigated and improved AudioController's stop-reason diagnostics
and OpenCV build compatibility using reproducible before-and-after tests.

Branch: assignment2/audio-stop-reasons
Pull Request: