cmake_minimum_required(VERSION 3.16)
project(camera_grab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(SFML 3 REQUIRED COMPONENTS Audio)

add_executable(camera_grab
    src/main.cpp
    src/CameraInput.cpp
    src/MarkerDetector.cpp
    src/MarkerRecognizer.cpp
    src/PoseEstimator.cpp
    src/AudioController.cpp
)

target_include_directories(camera_grab PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(camera_grab PRIVATE ${OpenCV_LIBS} SFML::Audio)

add_executable(test_audio_stop_reasons
    tests/test_audio_stop_reasons.cpp
    src/AudioController.cpp
)

target_include_directories(test_audio_stop_reasons PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(test_audio_stop_reasons PRIVATE SFML::Audio)

enable_testing()
add_test(NAME audio_stop_reasons COMMAND test_audio_stop_reasons)

add_executable(audio_test
    tests/audio_test.cpp
    src/AudioController.cpp
)

target_include_directories(audio_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(audio_test PRIVATE SFML::Audio)

# OpenCV 5 requires separate geometry declarations.
# Apply this only to the camera program.
if(OpenCV_VERSION VERSION_GREATER_EQUAL "5.0")
    if(MSVC)
        target_compile_options(camera_grab PRIVATE
            /FIopencv2/geometry/2d.hpp
        )
    else()
        target_compile_options(camera_grab PRIVATE
            -include opencv2/geometry/2d.hpp
        )
    endif()
endif()