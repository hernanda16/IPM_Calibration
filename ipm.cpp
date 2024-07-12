#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <yaml-cpp/yaml.h>

using namespace std;

//--Global variables
cv::Mat cameraMatrix, distCoeffs;
// cv::Point2f low_left = cv::Point2f(229, 403);
// cv::Point2f low_right = cv::Point2f(629, 419);
// cv::Point2f top_right = cv::Point2f(489, 335);
// cv::Point2f top_left = cv::Point2f(278, 333);

// cv::Point2f low_left = cv::Point2f(2, 378);
// cv::Point2f low_right = cv::Point2f(639, 378);
// cv::Point2f top_right = cv::Point2f(407, 262);
// cv::Point2f top_left = cv::Point2f(303, 261);

cv::Point2f low_left = cv::Point2f(2, 207);
cv::Point2f low_right = cv::Point2f(639, 209);
cv::Point2f top_right = cv::Point2f(395, 73);
cv::Point2f top_left = cv::Point2f(288, 71);

cv::Point2d center = cv::Point2d(650, 783);

//--Prototypes
void loadConfig();
void impTransform(cv::Mat& frame, cv::Mat& output);
void onMouse(int event, int x, int y, int flags, void* param);
double regress(double dist_on_frame);

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

        if (frame.empty()) {
            std::cerr << "Frame is empty!" << std::endl;
            break;
        }

        cv::Mat undistorted;
        cv::undistort(frame, undistorted, cameraMatrix, distCoeffs);

        cv::Mat output;
        impTransform(undistorted, output);

        // circle(output, center, 5, cv::Scalar(255, 255, 0), -1);
        // cv::line(output, cv::Point2d(center.x, 0), cv::Point2d(center.x, output.rows), cv::Scalar(255, 255, 0), 2, -1);
        // cv::line(output, cv::Point2d(0, center.y), cv::Point2d(output.cols, center.y), cv::Scalar(255, 255, 0), 2, -1);

        cv::imshow("Camera", frame);
        cv::imshow("Undistorted", undistorted);
        cv::imshow("IPM", output);

        // setMouseCallback("IPM", onMouse, &output);
        setMouseCallback("Undistorted", onMouse, &output);

        // Wait for a key event
        int key = cv::waitKey(1);
        if (key == 'q') {
            break;
        } else if (key == 'c') {
            cv::imwrite("../ipm.jpg", output);
        }
    }

    cap.release();

    return 0;
}

void impTransform(cv::Mat& frame, cv::Mat& output)
{
    cv::Point2f src[4] = { top_left, low_left, top_right, low_right };

    // Define the new width and height to cover a larger area
    int new_width = frame.cols * 2;
    int new_height = frame.rows * 2;

    // Adjust the destination points to cover the larger area
    /**
     * dst[0] = top_left
     * dst[1] = low_left
     * dst[2] = top_right
     * dst[3] = low_right
     */
    cv::Point2f dst[4] = { cv::Point2f(new_width / 4, new_height / 4),
        cv::Point2f(new_width / 4, 3 * new_height / 4),
        cv::Point2f(3 * new_width / 4, new_height / 4),
        cv::Point2f(3 * new_width / 4, 3 * new_height / 4) };

    cv::Mat M = cv::getPerspectiveTransform(src, dst);
    cv::warpPerspective(frame, output, M, cv::Size(new_width, new_height));
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

void onMouse(int event, int x, int y, int flags, void* param)
{
    if (event == cv::EVENT_LBUTTONDOWN) // Ketika tombol kiri mouse diklik
    {
        cv::Mat* img = reinterpret_cast<cv::Mat*>(param); // Ambil pointer ke gambar
        circle(*img, cv::Point(x, y), 5, cv::Scalar(0, 0, 255), -1); // Gambar lingkaran merah dengan radius 5
        printf("%.2f %.2f | %.2f -> %.2f\n", (x - center.x) * 0.48484848, (center.y - y) * 0.48484848, sqrt((x - center.x) * (x - center.x) + (y - center.y) * (y - center.y)), regress(sqrt((x - center.x) * (x - center.x) + (y - center.y) * (y - center.y))));
        imshow("IPM", *img); // Tampilkan gambar dengan lingkaran
    }

    // if (event == cv::EVENT_LBUTTONDOWN) // Ketika tombol kiri mouse diklik
    // {
    //     cv::Mat* img = reinterpret_cast<cv::Mat*>(param); // Ambil pointer ke gambar
    //     circle(*img, cv::Point(x, y), 5, cv::Scalar(0, 0, 255), -1); // Gambar lingkaran merah dengan radius 5
    //     printf("%d %d | %.2f -> %.2f\n", x, y, sqrt((x - 338) * (x - 338) + (477 - y) * (477 - y)), regress(sqrt((x - 338) * (x - 338) + (477 - y) * (477 - y))));
    //     // imshow("IPM", *img); // Tampilkan gambar dengan lingkaran
    // }
}

double regress(double x)
{
    double terms[] = {
        3.4299413372254179e+001,
        -2.0681007574227843e+000,
        1.3008098477025007e-001,
        -2.9138207361665922e-003,
        3.2496257325577386e-005,
        -1.9713396044159635e-007,
        6.6155296789731800e-010,
        -1.1531602736763176e-012,
        8.1518757950639620e-016
    };

    size_t csz = sizeof terms / sizeof *terms;

    double t = 1;
    double r = 0;
    for (int i = 0; i < csz; i++) {
        r += terms[i] * t;
        t *= x;
    }
    return r;
}