#pragma once

#include <functional>
#include <vector>

namespace utils {
    /// A callable event type that keeps track of callbacks to fire upon being
    /// triggered.
    ///
    /// \tparam E Context object to be passed to parameter-expecting callbacks.
    template <typename E> class event {
    public:
        /// Connects the event to a function `cb.`
        void connect(std::function<void()> const &cb)
        {
            simple_callbacks.push_back(cb);
        }

        /// Connects the event to a member-function pointer `cb`, binding it to
        /// `this_arg`.
        template <typename C>
        void connect(std::function<void (C::*)()> const &cb, C *this_arg)
        {
            simple_callbacks.push_back(std::bind(cb, this_arg));
        }

        /// Connects the event to a function `cb`, passing a context object to
        /// it.
        void connect(std::function<void(E const &)> const &cb)
        {
            using namespace std::placeholders;
            ctx_callbacks.push_back(cb, _1);
        }

        /// Connects the event to a member-function `cb`, binding it to
        /// `this_arg` and passing a context object to it.
        template <typename C>
        void connect(std::function<void (C::*)(E const &)> const &cb,
                     C *this_arg)
        {
            using namespace std::placeholders;
            ctx_callbacks.push_back(std::bind(cb, this_arg, _1));
        }

        /// Triggers the event, iterating through and calling each callback.
        void operator()(E ctx)
        {
            for (auto const &f : simple_callbacks) {
                f();
            }
            for (auto const &f : ctx_callbacks) {
                f(ctx);
            }
        }

    private:
        /// Stores callbacks that don't take an argument.
        ///
        /// Member-functions are bound before being stored.
        std::vector<std::function<void()>> simple_callbacks;
        /// Stores callbacks that do take an argument.
        ///
        /// Member-functions are bound before being stored.
        std::vector<std::function<void(E const &)>> ctx_callbacks;
    };
} // namespace utils
