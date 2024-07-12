#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

int main()
{
    // Open the default camera
    cv::VideoCapture cap("/dev/v4l/by-id/usb-e-con_systems_See3CAM_CU55_14205401-video-index0");
    if (!cap.isOpened()) {
        std::cout << "Failed to open the camera!" << std::endl;
        return -1;
    }

    // Create a folder to save the captured images in the current working directory
    std::string folderPath = "../captured_images";
    std::string command = "mkdir -p " + folderPath;
    system(command.c_str());

    // Capture and save an image when a key is pressed
    bool captureImage = false;
    cv::Mat frame;
    while (true) {
        cap >> frame;
        cv::imshow("Camera", frame);

        // Wait for a key event
        int key = cv::waitKey(1);
        if (key == 'c' || key == 'C') {
            captureImage = true;
        } else if (key == 27) { // 'Esc' key
            break;
        }

        if (captureImage) {
            // Generate a unique filename for each image
            static int imageCount = 0;
            std::string filename = folderPath + "/image_" + std::to_string(imageCount) + ".jpg";

            // Save the image
            cv::imwrite(filename, frame);

            std::cout << "Image captured and saved: " << filename << std::endl;

            captureImage = false;
            imageCount++;
        }
    }

    // Release the camera
    cap.release();

    return 0;
}