// NOT USED IN THE PROJECT

#include <iostream>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

// Defining the dimensions of checkerboard
// Make sure these match your actual checkerboard pattern
int CHECKERBOARD[2] { 8, 11 }; // Number of internal corners

int main()
{
    std::vector<std::vector<cv::Point3f>> objpoints; // 3D points
    std::vector<std::vector<cv::Point2f>> imgpoints; // 2D points

    // Define world coordinates for 3D points
    std::vector<cv::Point3f> objp;
    for (int i = 0; i < CHECKERBOARD[1] - 1; i++) {
        for (int j = 0; j < CHECKERBOARD[0] - 1; j++)
            objp.push_back(cv::Point3f(j, i, 0));
    }

    cv::Mat frame, gray;
    std::vector<cv::Point2f> corner_pts;
    bool success;

    for (int i = 0; i < 31; i++) {
        std::string path = "../captured_images/image_" + std::to_string(i) + ".jpg";
        frame = cv::imread(path);

        if (frame.empty()) {
            std::cout << "Image not found at path: " << path << std::endl;
            continue;
        }

        std::cout << "Processing image: " << path << std::endl;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Find checkerboard corners
        success = cv::findChessboardCorners(gray, cv::Size(CHECKERBOARD[0] - 1, CHECKERBOARD[1] - 1), corner_pts, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);

        if (success) {
            cv::TermCriteria criteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.001);
            cv::cornerSubPix(gray, corner_pts, cv::Size(11, 11), cv::Size(-1, -1), criteria);

            cv::drawChessboardCorners(frame, cv::Size(CHECKERBOARD[0] - 1, CHECKERBOARD[1] - 1), corner_pts, success);

            objpoints.push_back(objp);
            imgpoints.push_back(corner_pts);
        } else {
            std::cout << "Checkerboard corners not found in image: " << path << std::endl;
        }

        cv::imshow("Image", frame);
        cv::imshow("Gray Image", gray);
        if (cv::waitKey(0) == 'q') {
            continue;
        }
    }

    cv::destroyAllWindows();

    if (objpoints.size() < 10) {
        std::cerr << "Not enough valid images for calibration. Found only " << objpoints.size() << " valid images." << std::endl;
        return -1;
    }

    cv::Mat cameraMatrix, distCoeffs, R, T;
    cv::calibrateCamera(objpoints, imgpoints, cv::Size(gray.rows, gray.cols), cameraMatrix, distCoeffs, R, T);

    std::cout << "cameraMatrix : " << cameraMatrix << std::endl;
    std::cout << "distCoeffs : " << distCoeffs << std::endl;
    std::cout << "Rotation vector : " << R << std::endl;
    std::cout << "Translation vector : " << T << std::endl;

    return 0;
}
