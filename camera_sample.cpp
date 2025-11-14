#include <opencv2/opencv.hpp>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>

std::queue<cv::Mat> frameQueue;
std::mutex queueMutex;
std::condition_variable queueCond;

std::atomic<bool> running(true);

void captureThreadFunc() {
    cv::VideoCapture cap("/dev/video0", cv::CAP_V4L2);

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    if (!cap.isOpened()) {
        std::cerr << "カメラが開けません\n";
        running = false;
        return;
    }

    while (running) {
        cv::Mat frame;
        if (!cap.read(frame)) {
            std::cerr << "画像取得に失敗\n";
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            frameQueue.push(frame.clone());
        }
        queueCond.notify_one();
    }

    cap.release();
}

int main() {

    std::thread capThread(captureThreadFunc);

        std::unique_lock<std::mutex> lock(queueMutex);

        queueCond.wait(lock, [] { return !frameQueue.empty() || !running; });

        if (!running && frameQueue.empty()) break;

        cv::Mat frame = frameQueue.front();
        frameQueue.pop();
        lock.unlock();

        std::string filename = "capture_" + std::to_string(savedCount) + ".jpg";
        cv::imwrite(filename, frame);
        std::cout << filename << " を保存しました\n";

    running = false;
    queueCond.notify_all();
    capThread.join();

    return 0;
}
