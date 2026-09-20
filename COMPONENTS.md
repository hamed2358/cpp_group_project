# Audio Controller – Components

A – CameraInput → Fetches the camera image  
B – MarkerDetector → Finds the marker candidates   
C – MarkerRecognizer → Recognises the marker  
D – PoseEstimator → Calculates position and rotation  
E – AudioController → Controls audio, pitch, and volume
## Table for components
| Component | Responsibility | Input | Output | Primary owner | First evidence |
| :--- | :--- | :--- | :--- | :--- | :--- |
| CameraInput | Fetches the camera image | Device / video | `cv::Mat` (valid frame) | `Hamed` | Saved frame |
| MarkerDetector | Finds the marker candidates | `cv::Mat` (camera image) | Square + 4 corners | `Hamed` | Debug overlay |
| MarkerRecognizer | Recognises the marker and orientation | Square + corners | Approved marker + orientation | `Jonathan` | Pattern match test |
| PoseEstimator | Calculates position and rotation | Approved marker + corners | Position + rotation | `Abdulmajid` | Pose coordinates |
| AudioController | Controls audio, pitch, and volume | Position + rotation + visibility | Play/stop + pitch + volume | `Ali` | Audio output change |
## Component Responsibilities
### A – CameraInput

**Description:**  

CameraInput is the first part of the program and is responsible for communication with the camera. The component must open the camera, read new camera frames, and verify that the images are valid. Once a working image has been retrieved, it is passed on to the next component for further processing. If the camera cannot be opened or an image cannot be read, the component must be able to detect this and report the issue.

**Collaboration:**  

CameraInput primarily collaborates with MarkerDetector. CameraInput delivers the camera image, and MarkerDetector uses the image to start searching for the marker. The person responsible for CameraInput therefore needs to agree with the MarkerDetector owner on how the image should be passed between the components.

**Should not do:**  

CameraInput must not search for squares, recognise the marker, calculate the marker's position/rotation, or control the audio. Its responsibility ends once a working camera image has been passed on.

**Files:**  

CameraInput.hpp – the component's interface, i.e., what other parts of the program can use.  

CameraInput.cpp – the implementation itself that opens the camera and retrieves images.

**Header in CameraInput.cpp:**

```cpp
/*
Component: CameraInput
Primary owner: Hamed
Checkpoint: [Checkpoint]
Responsibility: Capture valid camera frames for the processing pipeline.
Main contributions: Camera setup, frame capture and input validation.
*/
```

**Short flow:**  

Camera → CameraInput → valid frame → MarkerDetector


### B – MarkerDetector

**Description:**  

MarkerDetector is responsible for receiving the camera image from CameraInput and searching for potential markers in the image. The component processes the image, finds regions and contours, and looks for shapes that could be a square marker. When a suitable square is found, the component must find its four corners and pass the information forward. It does not yet definitively state that the square is our marker, but rather finds potential marker candidates.

**Collaboration:**  

MarkerDetector receives the camera image from CameraInput and passes the found square and its corners on to MarkerRecognizer. The person responsible for MarkerDetector must therefore collaborate with both the CameraInput and MarkerRecognizer owners regarding what information is received and forwarded.

**Should not do:**  

MarkerDetector must not determine the marker's identity, calculate its pose, or control the audio. Its responsibility is primarily to find a suitable square and its corners.

**Files:**  

MarkerDetector.hpp – the component's interface, i.e., what other parts of the program can use.  

MarkerDetector.cpp – the implementation itself for image processing, contours, square validation, and corners.

**Header in MarkerDetector.cpp:**

```cpp
/*
Component: MarkerDetector
Primary owner: Hamed
Checkpoint: [Checkpoint]
Responsibility: Find marker candidates and their corners in the camera image.
Main contributions: Image processing, contour detection, quadrilateral validation and corner detection.
*/
```

**Short flow:**  

CameraInput → camera image → MarkerDetector → square + 4 corners → MarkerRecognizer

### C – MarkerRecognizer

**Description:**  

MarkerRecognizer is responsible for receiving the marker candidate from MarkerDetector and checking whether it truly is our marker. The component transforms the found square into a normalised, straight image and compares the pattern with the known marker pattern. If the pattern matches, the component must identify the marker and determine its orientation, i.e., how it is rotated.

**Collaboration:**  

MarkerRecognizer receives the square and its corners from MarkerDetector. Once the marker has been approved, information about it is sent on to PoseEstimator, which uses the corners, marker information, and camera information to calculate the marker's pose.

**Should not do:**  

MarkerRecognizer must not retrieve camera images, search for squares from scratch, calculate the final position/pose, or control the audio. Its main responsibility is to determine if the candidate is the correct marker and to establish its identity and orientation.

**Files:**  

MarkerRecognizer.hpp – the component's interface, i.e., what other parts of the program can use.  

MarkerRecognizer.cpp – the implementation itself for normalisation, comparison with known marker pattern, and recognition.

**Header in MarkerRecognizer.cpp:**

```cpp
/*
Component: MarkerRecognizer
Primary owner: [Name]
Checkpoint: [Checkpoint]
Responsibility: Recognise the marker and determine its identity and orientation.
Main contributions: Marker normalisation, pattern matching and orientation detection.
*/
```

**Short flow:**  

MarkerDetector → square + corners → MarkerRecognizer → approved marker + orientation → PoseEstimator


### D – PoseEstimator

**Description:**  

PoseEstimator is responsible for calculating where the marker is located and how it is oriented relative to the camera. The component uses information about the approved marker, its corners, and the camera's calibration data. The result is usable values for the marker's position and rotation, i.e., its pose. These values are needed for AudioController to link the marker's movement to the audio.

**Collaboration:**  

PoseEstimator receives the approved marker information from MarkerRecognizer. It then sends finished values, such as position/distance and rotation, on to AudioController.

**Should not do:**  

PoseEstimator must not open the camera, find squares, determine whether the candidate is the correct marker, or play/control the audio. Above all, it should convert the information about the already identified marker into usable position and rotation.

**Files:**  

PoseEstimator.hpp – the component's interface, i.e., what other parts of the program can use.  

PoseEstimator.cpp – the implementation itself for homography, camera transformation, and pose calculation/refinement.

**Header in PoseEstimator.cpp:**

```cpp
/*
Component: PoseEstimator
Primary owner: [Name]
Checkpoint: [Checkpoint]
Responsibility: Estimate the marker position and rotation relative to the camera.
Main contributions: Homography, camera transformation and pose estimation.
*/
```

**Short flow:**  

MarkerRecognizer → approved marker + corners → PoseEstimator → position/distance + rotation → AudioController


### E – AudioController

**Description:**  

AudioController is responsible for the actual sound and interaction in the project. The component receives information about the marker and uses it to control the audio according to our Mini Proposal. When the marker is visible, the audio should play, and when the marker disappears, the audio should stop. The marker's distance from the camera should be used to change the pitch, and the rotation can be used to change the volume.

**Collaboration:**  

AudioController primarily receives finished information from PoseEstimator, such as the marker's position/distance and rotation. It also needs to know if the marker is visible or has disappeared. The person responsible for AudioController therefore needs to collaborate with the PoseEstimator owner regarding which values should be sent to the audio part.

**Should not do:**  

AudioController must not open the camera, search for squares, recognise the marker, or calculate the marker's pose itself. It must use the finished marker information to control the audio.

**Files:**  

AudioController.hpp – the component's interface, i.e., what other parts of the program can use.  

AudioController.cpp – the implementation itself for playing/stopping audio and changing pitch and volume.

**Header in AudioController.cpp:**

```cpp
/*
Component: AudioController
Primary owner: [Name]
Checkpoint: [Checkpoint]
Responsibility: Control audio using information from the recognised marker.
Main contributions: Play/stop control, pitch control and volume control.
*/
```

**Short flow:**  

PoseEstimator → marker visible + distance + rotation → AudioController → play/stop + pitch + volume
## Diagram 
- **Camera / Video**
  - $\downarrow$
- **CameraInput** -Gets the camera image
  - $\downarrow$
- **MarkerDetector** -Finds the marker candidate
  - $\downarrow$
- **MarkerRecognizer** -Recognises the marker and orientation
  - $\downarrow$
- **PoseEstimator** -Calculates position and rotation
  - $\downarrow$
- **AudioController** -Controls audio, pitch, and volume
