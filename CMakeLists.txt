cmake_minimum_required(VERSION 3.16)
project(camera_grab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(SFML 3 REQUIRED COMPONENTS Audio)

add_executable(camera_grab
    main.cpp
    CameraInput.cpp
    MarkerDetector.cpp
    MarkerRecognizer.cpp
    PoseEstimator.cpp
    AudioController.cpp
)

target_include_directories(camera_grab PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(camera_grab PRIVATE ${OpenCV_LIBS} SFML::Audio)

add_executable(audio_test
    audio_test.cpp
    AudioController.cpp
)

target_link_libraries(audio_test PRIVATE SFML::Audio)