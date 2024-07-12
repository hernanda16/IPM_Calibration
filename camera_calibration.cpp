#include <iostream>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <stdio.h>

// Defining the dimensions of checkerboard
int CHECKERBOARD[2] { 7, 10 }; // Number of internal corners
const float square_size = 15.0f; // mm

int main()
{
    // Creating vector to store vectors of 3D points for each checkerboard image
    std::vector<std::vector<cv::Point3f>> objpoints;

    // Creating vector to store vectors of 2D points for each checkerboard image
    std::vector<std::vector<cv::Point2f>> imgpoints;

    // Define world coordinates for 3D points
    std::vector<cv::Point3f> objp;
    for (int i = 0; i < CHECKERBOARD[1]; i++) {
        for (int j = 0; j < CHECKERBOARD[0]; j++)
            objp.push_back(cv::Point3f(j * square_size, i * square_size, 0));
    }

    cv::Mat frame, gray;
    // vector to store the pixel coordinates of detected checker board corners
    std::vector<cv::Point2f> corner_pts;
    bool success;

    // Looping over all the images in the directory
    for (int i = 0; i < 60; i++) {
        std::string path = "../captured_images/image_" + std::to_string(i) + ".jpg";
        frame = cv::imread(path);

        if (frame.empty()) {
            std::cout << "Image not found at path: " << path << std::endl;
            continue;
        }

        printf("Image path: %s\n", path.c_str());
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Find checkerboard corners
        success = cv::findChessboardCorners(gray, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);

        /*
         * If desired number of corner are detected,
         * we refine the pixel coordinates and display
         * them on the images of checker board
         */
        if (success) {
            printf("Success\n");
            cv::TermCriteria criteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.001);

            // refining pixel coordinates for given 2d points.
            cv::cornerSubPix(gray, corner_pts, cv::Size(11, 11), cv::Size(-1, -1), criteria);

            cv::drawChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success);

            objpoints.push_back(objp);
            imgpoints.push_back(corner_pts);
        }

        cv::imshow("Image", frame);
        // Wait for 'q' key to be pressed
        if (cv::waitKey(0) == 'q') {
            continue;
        } else if (cv::waitKey(0) == 'p') {
            objpoints.pop_back();
            imgpoints.pop_back();
        }
    }

    cv::destroyAllWindows();

    cv::Mat cameraMatrix, distCoeffs, R, T;

    /*
     * Performing camera calibration by
     * passing the value of known 3D points (objpoints)
     * and corresponding pixel coordinates of the
     * detected corners (imgpoints)
     */
    cv::calibrateCamera(objpoints, imgpoints, cv::Size(gray.rows, gray.cols), cameraMatrix, distCoeffs, R, T);

    std::cout << "cameraMatrix : " << cameraMatrix << std::endl;
    std::cout << "distCoeffs : " << distCoeffs << std::endl;
    std::cout << "Rotation vector : " << R << std::endl;
    std::cout << "Translation vector : " << T << std::endl;

    return 0;
}