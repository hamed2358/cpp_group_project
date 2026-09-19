# Audio Controller – Components

## System Overview

Moving and rotating a physical marker in front of the camera controls audio. Showing or hiding the marker starts or stops the audio, moving the marker closer to or further from the camera changes the pitch, and rotating the marker changes the volume.

## Minimum Version

The minimum version of the system will:
- Start the audio when the marker is visible.
- Stop the audio when the marker is missing.
- Change the pitch based on the marker's distance from the camera.

Changing the volume by rotating the marker is an optional extension.

A – CameraInput → Fetches the camera image  
B – MarkerDetector → Finds the marker candidates   
C – MarkerRecognizer → Recognises the marker  
D – PoseEstimator → Calculates position and rotation  
E – AudioController → Controls audio, pitch, and volume
## Table for Components

| Component | What does it do? | What does it receive? | What does it send? | If it fails / finds nothing | Primary owner | First evidence |
|---|---|---|---|---|---|---|
| **CameraInput** | Gets valid camera frames | Camera or video | `cv::Mat` camera frame | No valid frame is sent | **Hamed** | Saved camera frame |
| **MarkerDetector** | Finds possible square markers | `cv::Mat` camera frame | Marker candidate + 4 corners | No marker candidate is sent | **Hamed** | Debug overlay showing detected square |
| **MarkerRecognizer** | Checks if the candidate is our marker and finds its orientation | Camera frame + marker candidate + 4 corners | Marker visibility + identity + orientation + 4 corners | Marker is reported as not recognised | **Jonathan** | Test showing correct pattern match |
| **PoseEstimator** | Calculates marker position and rotation | Recognised marker + 4 corners + camera calibration data | Position/distance + rotation | No valid pose is sent | **Abdulmajid** | Displayed pose values |
| **AudioController** | Uses marker information to control the audio | Visibility + distance + rotation | Play/stop + pitch + volume | If marker is missing, audio stops | **Ali** | Audible change in audio |
## Component Responsibilities
### A – CameraInput

**Description:**  

CameraInput is the first part of the program and is responsible for communication with the camera. The component must open the camera, read new camera frames, and verify that the images are valid. Once a working image has been retrieved, it is passed on to the next component for further processing. If the camera cannot be opened or an image cannot be read, the component must be able to detect this and report the issue.

**Collaboration:**  

CameraInput primarily collaborates with MarkerDetector. CameraInput delivers the camera image, and MarkerDetector uses the image to start searching for the marker. The person responsible for CameraInput therefore needs to agree with the MarkerDetector owner on how the image should be passed between the components.

**Should not do:**  

CameraInput must not search for squares, recognise the marker, calculate the marker's position/rotation, or control the audio. Its responsibility ends once a working camera image has been passed on.

**Initial Interface:**

- **Input:** Camera or video source.
- **Output:** A valid `cv::Mat` camera frame.
- **Failure:** If the camera cannot be opened or a frame cannot be read, no valid frame is passed to MarkerDetector.

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

**Initial Interface:**

- **Input:** A valid `cv::Mat` camera frame from CameraInput.
- **Output:** A marker candidate with its 4 corners.
- **Failure:** If no suitable square is found, no marker candidate is passed to MarkerRecognizer.

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

MarkerRecognizer receives the camera frame and the marker candidate with its corners. Once the marker has been approved, information about it is sent on to PoseEstimator, which uses the corners, marker information, and camera information to calculate the marker's pose.

**Should not do:**  

MarkerRecognizer must not retrieve camera images, search for squares from scratch, calculate the final position/pose, or control the audio. Its main responsibility is to determine if the candidate is the correct marker and to establish its identity and orientation.

**Initial Interface:**

- **Input:** The camera frame and a marker candidate with its 4 corners from MarkerDetector.
- **Output:** Marker visibility and, when recognised, the marker's identity, orientation, and 4 corners.
- **Failure:** If the candidate does not match the known marker pattern, marker visibility is set to false and no recognised marker is passed to PoseEstimator.

**Files:**  

MarkerRecognizer.hpp – the component's interface, i.e., what other parts of the program can use.  

MarkerRecognizer.cpp – the implementation itself for normalisation, comparison with known marker pattern, and recognition.

**Header in MarkerRecognizer.cpp:**

```cpp
/*
Component: MarkerRecognizer
Primary owner: Jonathan
Checkpoint: [Checkpoint]
Responsibility: Recognise the marker and determine its identity and orientation.
Main contributions: Marker normalisation, pattern matching and orientation detection.
*/
```

**Short flow:**  

MarkerDetector → camera frame + marker candidate + 4 corners → MarkerRecognizer → marker visibility + identity + orientation + 4 corners


### D – PoseEstimator

**Description:**  

PoseEstimator is responsible for calculating where the marker is located and how it is oriented relative to the camera. The component uses information about the approved marker, its corners, and the camera's calibration data. The result is usable values for the marker's position and rotation, i.e., its pose. These values are needed for AudioController to link the marker's movement to the audio.

**Collaboration:**  

PoseEstimator receives the approved marker information from MarkerRecognizer. It then sends finished values, such as position/distance and rotation, on to AudioController.

**Should not do:**  

PoseEstimator must not open the camera, find squares, determine whether the candidate is the correct marker, or play/control the audio. Above all, it should convert the information about the already identified marker into usable position and rotation.

**Initial Interface:**

- **Input:** A recognised marker with its 4 corners from MarkerRecognizer, together with the required camera calibration data.
- **Output:** The marker's position/distance and rotation relative to the camera.
- **Failure:** If a valid pose cannot be calculated, no pose values are passed to AudioController.

**Files:**  

PoseEstimator.hpp – the component's interface, i.e., what other parts of the program can use.  

PoseEstimator.cpp – the implementation itself for homography, camera transformation, and pose calculation/refinement.

**Header in PoseEstimator.cpp:**

```cpp
/*
Component: PoseEstimator
Primary owner: Abdulmajid
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

AudioController receives marker visibility from MarkerRecognizer and position/distance and rotation from PoseEstimator. The person responsible for AudioController therefore needs to collaborate with both the MarkerRecognizer and PoseEstimator owners regarding the information passed to the audio part.

**Should not do:**  

AudioController must not open the camera, search for squares, recognise the marker, or calculate the marker's pose itself. It must use the finished marker information to control the audio.

**Initial Interface:**

- **Input:** Marker visibility, position/distance, and rotation.
- **Output:** Audio play/stop, pitch, and volume control.
- **Failure / absence:** If the marker is missing or no valid marker information is available, the audio stops.

**Files:**  

AudioController.hpp – the component's interface, i.e., what other parts of the program can use.  

AudioController.cpp – the implementation itself for playing/stopping audio and changing pitch and volume.

**Header in AudioController.cpp:**

```cpp
/*
Component: AudioController
Primary owner: Ali
Checkpoint: [Checkpoint]
Responsibility: Control audio using information from the recognised marker.
Main contributions: Play/stop control, pitch control and volume control.
*/
```

**Short flow:**  

MarkerRecognizer → marker visibility → AudioController  
PoseEstimator → position/distance + rotation → AudioController → play/stop + pitch + volume

## Diagram 
- **Camera / Video**
  - $\downarrow$ camera or video input
- **CameraInput** - Gets the camera image
  - $\downarrow$ `cv::Mat` camera frame
- **MarkerDetector** - Finds the marker candidate
  - $\downarrow$ marker candidate + 4 corners
- **MarkerRecognizer** - Recognises the marker and determines marker visibility
  - $\rightarrow$ marker visibility → AudioController
  - $\downarrow$ recognised marker + identity + orientation + 4 corners → PoseEstimator
- **PoseEstimator** - Calculates position and rotation using camera calibration data
  - $\downarrow$ position/distance + rotation
- **AudioController** - Controls audio, pitch, and volume

## First Integration Target
**Components:** CameraInput → MarkerDetector

**Visible result:** CameraInput continuously sends valid camera frames to MarkerDetector, and MarkerDetector displays a simple processed debug view of the received frames.

**Done when:** The project builds successfully, CameraInput continuously provides frames to MarkerDetector, the processed debug view updates continuously, and the program closes cleanly.

**Integration coordinator:** Hamed