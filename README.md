#  High Performance Computing Architecture
To Apply OpenMP, MPI and GPU programming for the development of parallel programs.
Analyze and decompose the computation into parts for parallel computation.



### 🧠 Parallel Real-Time Face Detection using OpenMP and OpenCV

#### 🎯 Aim
Detect faces from the webcam in **real-time** and accelerate the detection process using **OpenMP (parallel computing)**.

---

### ⚙️ Tools & Technologies
- **OS:** Ubuntu 22.04  
- **Compiler:** g++  
- **Libraries:** OpenCV 4.x, OpenMP  
- **Model:** Haarcascade XML (for face detection)

---

### 🚀 How to Run
```bash
cd ~/face-detection
g++ face_parallel.cpp -o face_parallel `pkg-config --cflags --libs opencv4` -fopenmp
./face_parallel

🎬 Output
Webcam feed opens immediately.
Green rectangles mark detected faces.
FPS (Frames Per Second) table appears in the terminal showing performance for 1, 2, and 4 threads.
[Parallel Face Detection Results]
---------------------------------
Threads | Avg FPS | Avg Faces Detected
---------------------------------
1       | 11.64   | 1.10
2       | 11.65   | 1.18
4       | 12.14   | 1.12
---------------------------------
Press ESC to quit the interface.

Observations:
FPS increases slightly with more threads.
CPU utilization improves due to parallel frame processing.
Demonstrates how OpenMP enhances performance in real-time computer vision tasks.

🏁 Conclusion
This project successfully showcases how parallel computing (OpenMP) can boost the speed of real-time face detection using OpenCV.

