#include "image_loop.hpp"

controllers::image_loop::image_loop(controllers::mqtt &comms) noexcept
    : capture(0), comms(comms), running(true)
{
    constexpr auto width = 1280.F;
    constexpr auto height = 720.F;
    constexpr auto fps = 24.F;
    capture.set(cv::CAP_PROP_FRAME_WIDTH, width);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    capture.set(cv::CAP_PROP_FPS, fps);

    t = std::thread([](decltype(this) self) { (*self)(); }, this);
}

controllers::image_loop::~image_loop()
{
    // Technically, we'd need to check if joinable and running, but since these
    // won't be set to false anywhere other than here, it's unnecessary.
    running = false;
    t.join();
}

void controllers::image_loop::operator()()
{
    using namespace std::literals;

    cv::Mat cap_buf;
    cv::Mat reshaped;

    constexpr auto waiting_period = 42ms;

    while (running && !comms.is_connected()) {
        std::this_thread::sleep_for(waiting_period);
    }

    while (running) {
        if (!comms.is_connected()) {
            continue;
        }
        capture >> cap_buf;
        reshaped = cap_buf.reshape(
            1, static_cast<int>(cap_buf.total() * cap_buf.channels()));
        std::string send_buf(reshaped.datastart, reshaped.dataend);

        comms.publish_message(std::string(cam_topic), send_buf, 0);

        std::this_thread::sleep_for(waiting_period);
    }
}
