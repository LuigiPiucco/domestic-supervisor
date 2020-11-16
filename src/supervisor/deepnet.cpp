#include "deepnet.hpp"

controllers::deepnet::deepnet()
    : running(true), enabled(false), prev_detections(0)
{
    dlib::deserialize("assets/mmod_human_face_detector.dat") >> net;

    t = std::thread([](decltype(this) self) { (*self)(); }, this);
}

controllers::deepnet::~deepnet()
{
    running = false;
    t.join();
}

void controllers::deepnet::enqueue(std::string &&im)
{
    std::scoped_lock l{processing_queue_mutex};
    processing_queue.emplace(im);
}

auto controllers::deepnet::processed() noexcept -> utils::event<image> &
{
    return _processed;
}

auto controllers::deepnet::changed_num_detections() noexcept
    -> utils::event<int> &
{
    return _changed_num_detections;
}

void controllers::deepnet::set_enabled() noexcept { enabled = true; }

void controllers::deepnet::reset_enabled() noexcept { enabled = false; }

void controllers::deepnet::toggle_enabled() noexcept { enabled = !enabled; }

auto controllers::deepnet::state_enabled() const noexcept -> bool
{
    return enabled;
}

void controllers::deepnet::set_processing() noexcept { processing = true; }

void controllers::deepnet::reset_processing() noexcept { processing = false; }

void controllers::deepnet::toggle_processing() noexcept
{
    processing = !processing;
}

auto controllers::deepnet::state_processing() const noexcept -> bool
{
    return processing;
}

void controllers::deepnet::operator()()
{
    while (running) {
        if (std::scoped_lock l{processing_queue_mutex};
            !enabled || processing_queue.empty()) {
            continue;
        }

        constexpr int max_frames = 20;
        if (std::scoped_lock l{processing_queue_mutex};
            processing_queue.size() > max_frames) {
            processing_queue = std::queue<std::string>{};
            continue;
        }

        image mat;
        {
            std::scoped_lock l{processing_queue_mutex};

            auto const &frame = processing_queue.front();
            for (int i = 0; i < height; ++i) {
                for (int j = 0; j < width; ++j) {
                    auto const first_byte = 3 * (i * width + j);
                    dlib::rgb_pixel pix(frame.at(first_byte + 2),
                                        frame.at(first_byte + 1),
                                        frame.at(first_byte + 0));
                    mat(i, j) = pix;
                }
            }

            processing_queue.pop();
        }

        if (processing) {
            auto detections =
                net(static_cast<detector::input_type const &>(mat));
            if (detections.size() != prev_detections) {
                _changed_num_detections(detections.size());
                prev_detections = detections.size();
            }
            dlib::rgb_pixel const red(255, 85, 85);
            for (auto const &det : detections) {
                dlib::draw_rectangle(mat, det.rect, red, 4);
            }
        }

        _processed(mat);
    }
}