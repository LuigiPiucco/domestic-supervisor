#pragma once

#include "mqtt.hpp"
#include <atomic>
#include <chrono>
#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>
#include <thread>
#include <vector>

namespace controllers {
    class image_loop {
    public:
        image_loop(controllers::mqtt &comms) noexcept;
        image_loop(image_loop const &il) = delete;
        image_loop(image_loop &&il) = delete;
        auto operator=(image_loop const &il) -> image_loop & = delete;
        auto operator=(image_loop &&il) -> image_loop & = delete;
        ~image_loop();

    private:
        void operator()();

        cv::VideoCapture capture;
        controllers::mqtt &comms;
        std::thread t;
        std::atomic_bool running;
    };
} // namespace controllers
