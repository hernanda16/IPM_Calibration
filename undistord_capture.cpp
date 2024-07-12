#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <yaml-cpp/yaml.h>

using namespace std;

//--Global variables
cv::Mat cameraMatrix, distCoeffs;

//--Prototypes
void loadConfig();

int main()
{
    loadConfig();

    // Open the default camera
    cv::VideoCapture cap("/dev/v4l/by-id/usb-e-con_systems_See3CAM_CU55_14205401-video-index0");
    if (!cap.isOpened()) {
        std::cout << "Failed to open the camera!" << std::endl;
        return -1;
    }

    while (true) {
        cv::Mat frame;
        cap >> frame;

        cv::Mat undistorted;
        cv::undistort(frame, undistorted, cameraMatrix, distCoeffs);

        cv::imshow("Camera", frame);
        cv::imshow("Undistorted", undistorted);

        // Wait for a key event
        int key = cv::waitKey(1);
        if (key == 'q') {
            break;
        } else if (key == 'c') {
            cv::imwrite("../undistorted.jpg", undistorted);
        }
    }

    cap.release();

    return 0;
}

void loadConfig()
{
    char config_file[100];
    sprintf(config_file, "../camera_calibration.yaml");

    YAML::Node config = YAML::LoadFile(config_file);

    /**
     * camera_matrix:
     * rows: 3
     * cols: 3
     * [fx 0 cx]
     * [0 fy cy]
     * [0  0  1]
     */
    cameraMatrix = (cv::Mat_<double>(3, 3) << config["cameraMatrix"]["data"][0].as<double>(), config["cameraMatrix"]["data"][1].as<double>(), config["cameraMatrix"]["data"][2].as<double>(),
        config["cameraMatrix"]["data"][3].as<double>(), config["cameraMatrix"]["data"][4].as<double>(), config["cameraMatrix"]["data"][5].as<double>(),
        config["cameraMatrix"]["data"][6].as<double>(), config["cameraMatrix"]["data"][7].as<double>(), config["cameraMatrix"]["data"][8].as<double>());

    /**
     * distortion_coefficients:
     * rows: 1
     * cols: 5
     * [k1 k2 p1 p2 k3]
     */
    distCoeffs = (cv::Mat_<double>(1, 5) << config["distCoeffs"]["data"][0].as<double>(), config["distCoeffs"]["data"][1].as<double>(), config["distCoeffs"]["data"][2].as<double>(),
        config["distCoeffs"]["data"][3].as<double>(), config["distCoeffs"]["data"][4].as<double>());
}
