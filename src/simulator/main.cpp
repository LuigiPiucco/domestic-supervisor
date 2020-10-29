#include <chrono>
#include <mqtt/buffer_view.h>
#include <mqtt/callback.h>
#include <mqtt/client.h>
#include <mqtt/connect_options.h>
#include <mqtt/delivery_token.h>
#include <mqtt/message.h>
#include <mqtt/properties.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <spdlog/spdlog.h>
#include <string>

const std::string address{"tcp://127.0.0.1:14442/"};
const std::string client_id{"house_data_publisher"};
const std::string cam_topic{"camera/video"};
constexpr auto qos = 0;
constexpr auto ret = false;

class UserCallback : public virtual mqtt::callback {

    void connection_lost(std::string const &cause) override
    {
        spdlog::error("Conexão perdida! Causa:\n{0}", cause);
        std::exit(EXIT_FAILURE);
    }
    void delivery_complete(mqtt::delivery_token_ptr tok) override {}
};

int main()
{
    using namespace std::literals;

    mqtt::connect_options conn_opts{};
    constexpr auto interval = 20;
    conn_opts.set_keep_alive_interval(interval);
    conn_opts.set_max_inflight(4);
    conn_opts.set_clean_start(true);
    conn_opts.set_clean_session(true);
    conn_opts.set_mqtt_version(MQTTVERSION_5);

    cv::VideoCapture capture(0);
    constexpr auto width = 1280.F;
    constexpr auto height = 720.F;
    constexpr auto fps = 24.F;
    capture.set(cv::CAP_PROP_FRAME_WIDTH, width);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    capture.set(cv::CAP_PROP_FPS, fps);
    cv::Mat mat_buffer{};
    cv::Mat rgba_buffer{};
    std::vector<uchar> byte_buffer{};

    mqtt::client client(address, client_id);
    UserCallback cb;
    client.set_callback(cb);
    client.connect(conn_opts);

    while (!client.is_connected()) {
    }

    while (true) {
        capture >> mat_buffer;

        auto reshaped = mat_buffer.reshape(
            1, static_cast<int>(mat_buffer.total() * mat_buffer.channels()));
        reshaped.copyTo(byte_buffer);
        auto msg = mqtt::message::create(cam_topic, byte_buffer.data(),
                                         byte_buffer.size(), qos, ret);
        client.publish(msg);
        std::this_thread::sleep_for(42ms);
    }

    return 0;
}
