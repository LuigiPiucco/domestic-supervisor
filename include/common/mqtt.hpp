#pragma once

#include "event.hpp"
#include <chrono>
#include <cstdlib>
#include <initializer_list>
#include <mqtt/async_client.h>
#include <mqtt/callback.h>
#include <mqtt/iaction_listener.h>
#include <mqtt/message.h>
#include <spdlog/spdlog.h>
#include <string_view>
#include <thread>

constexpr std::string_view address{"tcp://127.0.0.1:14442/"};
constexpr std::string_view simulator_client_id{"house_data_publisher"};
constexpr std::string_view supervisor_client_id{"house_data_consumer"};
constexpr std::string_view cam_topic{"domestic/video"};
constexpr std::string_view state_topic{"domestic/state_change"};
constexpr std::string_view ack_topic{"domestic/state_acknowledge"};

namespace controllers {
    class mqtt final : public virtual ::mqtt::callback,
                       public virtual ::mqtt::iaction_listener {
    public:
        class incoming_event {
        public:
            incoming_event(::mqtt::const_message_ptr message_ptr);

            [[nodiscard]] auto topic() const noexcept -> std::string_view;
            [[nodiscard]] auto message() const noexcept -> std::string_view;

        private:
            /// Needed to keep the shared reference to the MQTT message alive
            /// as long as something may access `message` or `topic`;
            ::mqtt::const_message_ptr message_ptr;
        };

        mqtt(std::string_view id,
             std::initializer_list<std::string_view> topics);
        mqtt(mqtt const &m) = delete;
        mqtt(mqtt &&m) = delete;
        auto operator=(mqtt const &m) -> mqtt & = delete;
        auto operator=(mqtt &&m) noexcept -> mqtt & = delete;
        ~mqtt() final = default;

        void publish_message(std::string const &topic,
                             std::string const &message, int qos);

        [[nodiscard]] auto incoming() noexcept
            -> utils::event<incoming_event> &;

        [[nodiscard]] auto is_connected() noexcept -> bool;

        void disconnect();

    private:
        ::mqtt::async_client client;
        utils::event<incoming_event> _incoming;

        void on_failure(::mqtt::token const &tok) final;
        void on_success(::mqtt::token const &tok) final;
        void connected(std::string const &cause) final;
        void message_arrived(::mqtt::const_message_ptr msg) final;
    };
} // namespace controllers
