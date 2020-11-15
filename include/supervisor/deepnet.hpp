#pragma once

#include "event.hpp"
#include <array>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/traits.hpp>
#include <boost/range.hpp>
#include <dlib/dnn.h>
#include <mutex>
#include <queue>
#include <thread>

static constexpr int width = 1280;
static constexpr int height = 720;
constexpr std::size_t bmp_header_size = 54;
constexpr int bmp_filesize = bmp_header_size + 3 * width * height;

constexpr std::array<unsigned char, bmp_header_size> bmp_header = {
    'B',
    'M',
    (bmp_filesize >> 0) & 0xFF,
    (bmp_filesize >> 1) & 0xFF,
    (bmp_filesize >> 2) & 0xFF,
    (bmp_filesize >> 3) & 0xFF,
    0,
    0,
    0,
    0,
    54,
    0,
    0,
    0,
    40,
    0,
    0,
    0,
    (width >> 0) & 0xFF,
    (width >> 1) & 0xFF,
    (width >> 2) & 0xFF,
    (width >> 3) & 0xFF,
    (height >> 0) & 0xFF,
    (height >> 1) & 0xFF,
    (height >> 2) & 0xFF,
    (height >> 3) & 0xFF,
    1,
    0,
    24,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

namespace controllers {
    class deepnet {
        template <long num_filters, typename SUBNET>
        using con5d = dlib::con<num_filters, 5, 5, 2, 2, SUBNET>;
        template <long num_filters, typename SUBNET>
        using con5 = dlib::con<num_filters, 5, 5, 1, 1, SUBNET>;

        template <typename SUBNET>
        using downsampler = dlib::relu<dlib::affine<
            con5d<32, dlib::relu<dlib::affine<con5d<
                          32, dlib::relu<dlib::affine<con5d<16, SUBNET>>>>>>>>>;
        template <typename SUBNET>
        using rcon5 = dlib::relu<dlib::affine<con5<45, SUBNET>>>;

    public:
        using detector = dlib::loss_mmod<dlib::con<
            1, 9, 9, 1, 1,
            rcon5<rcon5<rcon5<downsampler<
                dlib::input_rgb_image_pyramid<dlib::pyramid_down<6>>>>>>>>;
        using image = dlib::matrix<dlib::rgb_pixel, height, width>;
        using detections_type = class std::vector<struct dlib::mmod_rect>;

        deepnet();
        deepnet(deepnet const &d) = delete;
        deepnet(deepnet &&d) = delete;
        auto operator=(deepnet const &d) -> deepnet & = delete;
        auto operator=(deepnet &&d) -> deepnet & = delete;
        ~deepnet();

        void enqueue(std::string &&im);
        [[nodiscard]] auto processed() noexcept -> utils::event<image> &;
        [[nodiscard]] auto changed_num_detections() noexcept
            -> utils::event<int> &;

        void set_enabled() noexcept;
        void reset_enabled() noexcept;
        void toggle_enabled() noexcept;
        [[nodiscard]] auto state_enabled() const noexcept -> bool;

        void set_processing() noexcept;
        void reset_processing() noexcept;
        void toggle_processing() noexcept;
        [[nodiscard]] auto state_processing() const noexcept -> bool;

    private:
        void operator()();

        std::mutex processing_queue_mutex;
        std::queue<std::string> processing_queue;
        std::atomic_bool running;
        std::atomic_bool enabled;
        std::atomic_bool processing;
        std::thread t;
        utils::event<image> _processed;
        utils::event<int> _changed_num_detections;
        detector net;
        std::atomic_int prev_detections;
    };
} // namespace controllers