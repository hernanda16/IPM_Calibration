#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// Fungsi callback untuk menangani event mouse
void onMouse(int event, int x, int y, int flags, void* param)
{
    if (event == EVENT_LBUTTONDOWN) // Ketika tombol kiri mouse diklik
    {
        Mat* img = reinterpret_cast<Mat*>(param); // Ambil pointer ke gambar
        circle(*img, Point(x, y), 10, Scalar(0, 0, 255), -1); // Gambar lingkaran merah dengan radius 10
        printf("Clicked at (%d, %d)\n", x, y);
        imshow("Image", *img); // Tampilkan gambar dengan lingkaran
    }
}

int main()
{
    // Baca gambar dari file
    Mat frame = imread("../undistorted.jpg");
    if (frame.empty()) {
        cerr << "Image not found!" << endl;
        return -1;
    }

    // Tampilkan gambar
    imshow("Image", frame);

    // Set callback untuk event mouse
    setMouseCallback("Image", onMouse, &frame);

    // Tunggu hingga pengguna menekan tombol
    waitKey(0);

    return 0;
}
