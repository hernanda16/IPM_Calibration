cmake_minimum_required(VERSION 3.12)
project(camera_calibration)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Find Yaml package
find_package(yaml-cpp REQUIRED)

# Add an executable target
add_executable(capture_checkerboard capture_checkerboard.cpp)
add_executable(camera_calibration camera_calibration.cpp)
add_executable(undistord_capture undistord_capture.cpp)
add_executable(trapezoidal_clicker trapezoidal_clicker.cpp)
add_executable(ipm ipm.cpp)

# Link against OpenCV libraries
target_link_libraries(capture_checkerboard ${OpenCV_LIBS})
target_link_libraries(camera_calibration ${OpenCV_LIBS})
target_link_libraries(undistord_capture ${OpenCV_LIBS} ${YAML_CPP_LIBRARIES})
target_link_libraries(trapezoidal_clicker ${OpenCV_LIBS})
target_link_libraries(ipm ${OpenCV_LIBS} ${YAML_CPP_LIBRARIES})