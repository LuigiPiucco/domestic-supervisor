#include "mqtt.hpp"

void controllers::mqtt::publish_message(std::string const &topic,
                                        std::string const &message, int qos)
{
    auto msg = ::mqtt::message::create(topic, message, qos, false);
    client.publish(msg);
}

auto controllers::mqtt::incoming() noexcept
    -> utils::event<controllers::mqtt::incoming_event> &
{
    return _incoming;
}

controllers::mqtt::mqtt(std::string_view id,
                        std::initializer_list<std::string_view> topics)
    : client(address.data(), id.data(), std::string("/tmp/mqtt-").append(id))
{
    using namespace std::literals;

    constexpr auto interval = 20ms;
    auto conn_opts = ::mqtt::connect_options_builder()
        .clean_session()
        .keep_alive_interval(interval)
        .max_inflight(4)
        .finalize();

    client.set_callback(*this);
    auto conn_tok = client.connect(conn_opts);
    conn_tok->wait();

    for (auto const &topic : topics) {
        client.subscribe(std::string(topic), 0);
    }
}

auto controllers::mqtt::is_connected() noexcept -> bool
{
    return client.is_connected();
}

void controllers::mqtt::disconnect() { client.disconnect(); }

void controllers::mqtt::on_failure(::mqtt::token const &tok)
{
    spdlog::error("Failed to connect to the MQTT Broker. Exiting.");
    std::exit(EXIT_FAILURE);
}

void controllers::mqtt::on_success(::mqtt::token const &tok) {}
void controllers::mqtt::connected(std::string const &cause)
{
    spdlog::info("Succeeded to connect to the MQTT Broker.");
    client.subscribe(state_topic.data(), 2);
}
void controllers::mqtt::message_arrived(::mqtt::const_message_ptr msg)
{
    mqtt::incoming_event _incoming_event(msg);

    _incoming(_incoming_event);
}

controllers::mqtt::incoming_event::incoming_event(
    ::mqtt::const_message_ptr message_ptr)
    : message_ptr(std::move(message_ptr))
{
}

auto controllers::mqtt::incoming_event::topic() const noexcept
    -> std::string_view
{
    return message_ptr->get_topic();
}

auto controllers::mqtt::incoming_event::message() const noexcept
    -> std::string_view
{
    return message_ptr->get_payload_str();
}
