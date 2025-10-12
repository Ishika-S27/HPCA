#include <opencv2/opencv.hpp>
#include <omp.h>
#include <vector>
#include <iostream>

int main() {
    // Open webcam using V4L2 backend to avoid GStreamer delay
    cv::VideoCapture cap(0, cv::CAP_V4L2);
    if (!cap.isOpened()) {
        std::cerr << "Cannot open webcam\n";
        return -1;
    }

    // Reduce internal buffering to decrease latency
    cap.set(cv::CAP_PROP_BUFFERSIZE, 1);

    cv::CascadeClassifier face_cascade;
    if (!face_cascade.load("/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml")) {
        std::cerr << "Cannot load Haar cascade xml\n";
        return -1;
    }

    cv::Mat frame, gray;

    // Show the window immediately
    cv::namedWindow("Real-time Face Detection", cv::WINDOW_AUTOSIZE);

    // Test with different thread counts (FPS table)
    std::vector<int> thread_tests = {1, 2, 4};  // you can add 8, 16 if CPU supports
    int test_frames = 50;

    std::cout << "\n[Parallel Face Detection Results]\n";
    std::cout << "---------------------------------\n";
    std::cout << "Threads | Avg FPS | Avg Faces Detected\n";
    std::cout << "---------------------------------\n";

    for (int t : thread_tests) {
        omp_set_num_threads(t);

        double total_fps = 0.0;
        double total_faces = 0.0;

        for (int i = 0; i < test_frames; i++) {
            cap >> frame;
            if (frame.empty()) break;

            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

            std::vector<cv::Rect> faces;
            double t_start = omp_get_wtime();

            #pragma omp parallel
            {
                std::vector<cv::Rect> local_faces;
                #pragma omp for nowait
                for (int j = 0; j < 1; j++) {
                    face_cascade.detectMultiScale(gray, local_faces, 1.1, 3, 0, cv::Size(30, 30));
                }

                #pragma omp critical
                faces.insert(faces.end(), local_faces.begin(), local_faces.end());
            }

            double t_end = omp_get_wtime();
            double fps = 1.0 / (t_end - t_start);

            total_fps += fps;
            total_faces += faces.size();

            // Draw faces in real-time during FPS measurement
            for (auto &f : faces) {
                cv::rectangle(frame, f, cv::Scalar(0, 255, 0), 2);
            }
            cv::imshow("Real-time Face Detection", frame);
            if (cv::waitKey(1) == 27) break; // ESC to quit
        }

        std::cout << t << "       | "
                  << (total_fps / test_frames) << "   | "
                  << (total_faces / test_frames) << "\n";
    }

    std::cout << "---------------------------------\n";
    std::cout << "ESC to quit real-time demo...\n";

    // Run continuous real-time detection with max threads
    omp_set_num_threads(omp_get_max_threads());
    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        std::vector<cv::Rect> faces;
        face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0, cv::Size(30, 30));

        for (auto &f : faces) {
            cv::rectangle(frame, f, cv::Scalar(0, 255, 0), 2);
        }

        cv::imshow("Real-time Face Detection", frame);
        if (cv::waitKey(1) == 27) break; // ESC to quit
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
