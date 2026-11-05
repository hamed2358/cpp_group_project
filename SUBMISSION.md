## PoseEstimator
PoseEstimator is used to calculate the marker’s 3D position and distance from the camera. We use OpenCV’s solvePnP function together with the marker’s four corners, the markers real size, and the camera calibration values. The distance is calculated using distanceToMarker and then sent to AudioController.

## Limitations
The camera calibration was performed using the camera of a MacBook Air 2022. Using another camera may affect the accuracy of the distance calculation. The result also depends on detecting the marker’s four corners correctly.



## AudioController
AudioController uses SFML 3 to generate and control sound. When the marker moves closer to the camera, the pitch becomes higher. When it moves farther away, the pitch becomes lower. The marker’s rotation is used to change the volume. The sound stops automatically when the marker is no longer detected or when invalid values are received.

## Limitations
The sound pitch only changes within a distance range of 0.2 to 1.0 meters.