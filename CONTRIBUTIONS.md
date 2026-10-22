
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

## Abdulmajid Almohamed
### PoseEstimator
Files:
- PoseEstimator.hpp
- PoseEstimator.cpp
Responsible for estimating the position, distance and rotation of the marker relative to the camera.
Branch: feature/pose-estimator  
Pull Request: https://github.com/hamed2358/cpp_group_project/pull/7


## Ali Akartei

### Assignment 2 – PoseEstimator input validation

Added validation that rejects non-finite image-corner coordinates
before calling solvePnP. The public interface and pose calculation
remain unchanged.

Added three automated tests: valid corners, a NaN coordinate and
an infinite coordinate. Before the change, 1/3 tests passed;
after the change, 3/3 passed.

Files:
- PoseEstimator.cpp
- pose_test.cpp
- CMakeLists.txt

Evidence:
- docs/test-evidence/pose_before.txt
- docs/test-evidence/pose_after.txt

Branch: assignment2/pose-review
