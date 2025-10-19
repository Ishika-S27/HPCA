#include <iostream>
#include <opencv2/opencv.hpp>
#include <omp.h>

using namespace std;
using namespace cv;

int main() {
    // Load input image (grayscale)
    Mat image = imread("input.jpg", IMREAD_GRAYSCALE);
    if (image.empty()) {
        cout << "Error: Image not found!" << endl;
        return -1;
    }

    Mat output = image.clone();

    // 3x3 Gaussian kernel
    float kernel[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
    };

    // Normalize kernel
    float sum = 0.0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            sum += kernel[i][j];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            kernel[i][j] /= sum;

    double start = omp_get_wtime();

    // Parallel Gaussian blur using OpenMP
    #pragma omp parallel for collapse(2)
    for (int i = 1; i < image.rows - 1; i++) {
        for (int j = 1; j < image.cols - 1; j++) {
            float pixel = 0.0;
            for (int k = -1; k <= 1; k++) {
                for (int l = -1; l <= 1; l++) {
                    pixel += kernel[k + 1][l + 1] * image.at<uchar>(i + k, j + l);
                }
            }
            output.at<uchar>(i, j) = (uchar)pixel;
        }
    }

    double end = omp_get_wtime();
    cout << "Execution Time (Parallel): " << end - start << " seconds" << endl;

    // Display input and output images
    imshow("Input Image", image);       // show original
    imshow("Blurred Image", output);    // show blurred
    cout << "Press any key to close the images..." << endl;
    waitKey(0);                         // wait until key press
    destroyAllWindows();                // close windows

    // Save the output image
    imwrite("blurred_output.jpg", output);
    cout << "Blurred image saved as 'blurred_output.jpg'" << endl;

    return 0;
}

g++ -fopenmp gaussian_blur.cpp -o gaussian_blur `pkg-config --cflags --libs opencv4`
  
./gaussian_blur
