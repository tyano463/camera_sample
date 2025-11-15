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
        queueCond.notify_all();
        return;
    }

    while (running) {
        cv::Mat frame;
        if (!cap.read(frame)) {
            std::cerr << "画像取得に失敗\n";
            continue;
        }

        // ---- push 時のみ短時間ロック ----
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            frameQueue.push(frame.clone());
        }
        queueCond.notify_one();  // wait を起こす
    }

    cap.release();
}

int main() {
    std::thread capThread(captureThreadFunc);

    cv::Mat frame;
    {
        std::unique_lock<std::mutex> lock(queueMutex);

        // wait は pthread_cond_wait と同じ動き：
        // → mutex を unlock して待機
        // → 起こされたら mutex を再 lock して戻る
        queueCond.wait(lock, [] {
            return !frameQueue.empty() || !running;
        });

        if (!frameQueue.empty()) {
            frame = frameQueue.front().clone();
            frameQueue.pop();
        }
    }  // lock がここで自動 unlock（C の pthread_mutex_unlock と同じ）

    if (!frame.empty()) {
        std::string filename = "captured.jpg";
        cv::imwrite(filename, frame);
        std::cout << filename << " を保存しました\n";
    }

    running = false;
    queueCond.notify_all();
    capThread.join();

    return 0;
}

