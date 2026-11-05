Project Description:
This project is a C++ application that uses a camera to detect and recognize a custom marker. The marker is used to control sound through its distance from the camera and its rotation.
Moving the marker closer to the camera increases the pitch, while moving it further away decreases the pitch. Rotating the marker changes the volume. The sound stops when the marker is no longer detected.

System Requirements:
The project has been built and tested on macOS (Apple Silicon) using Apple Clang 17.0.0 and on Windows 11 using MSVC (Microsoft Visual C++).
The project uses C++17 and requires CMake 3.16 or newer.

Required Libraries:
The project uses the following libraries:
 OpenCV 5.0.0 for camera input, marker detection, recognition and pose estimation.
 SFML 3.0.2 for sound generation and playback.

 Build and Run:
Open a terminal in the project folder and run the following commands:
1. Configure the project: cmake -S . -B build
2. Build the project:     cmake --build build
3. Run the application:   ./build/camera_grab
On Windows 11 with MSVC, the executable is normally located at:
build/Debug/camera_grab.exe

How to Test the Application:
After starting the program, show the marker in front of the camera.
1. Move the marker closer to the camera to increase the pitch.
2. Move the marker further away to decrease the pitch.
3. Rotate the marker clockwise to increase the volume.
4. Rotate the marker counterclockwise to decrease the volume.
5. Remove the marker from the camera view to stop the sound.
Press Esc to close the program.

Camera Requirements:
The application uses a camera to detect the marker. We have tested it with the built-in cameras on MacBook and HP laptops, as well as phone cameras connected to a Mac. The program uses camera index 0 by default.

Camera Calibration:
The camera calibration values are stored in CameraCalibration.cpp. These values are used by PoseEstimator to calculate the marker’s position and distance from the camera.

The marker image is available in resources/marker.png. The marker size is set to 0.043 meters (4.3 cm) in main.cpp. This size is used by PoseEstimator to calculate the distance between the camera and the marker.


