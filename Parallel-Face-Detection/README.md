# Parallel Real-Time Face Detection Project

**Aim:** Detect faces from the webcam in real-time and speed up detection using OpenMP (parallel computing).

**Tools Used:** Ubuntu 22.04, g++, OpenCV 4.x, OpenMP, Haarcascade XML.

## How to Run
```bash
cd ~/face-detection/Parallel-Face-Detection
g++ face_parallel.cpp -o face_parallel `pkg-config --cflags --libs opencv4` -fopenmp
./face_parallel

Output

Webcam feed appears immediately with green rectangles on detected faces.

FPS (frames per second) table printed in terminal showing performance for 1, 2, 4 threads:

Threads	Avg FPS	Avg Faces Detected
1	11.64	1.1
2	11.65	1.18
4	12.14	1.12

Press ESC to quit the interface.

Notes:

FPS increases with more threads.

CPU utilization improves with parallel execution.
