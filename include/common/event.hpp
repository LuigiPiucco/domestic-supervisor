#pragma once

#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace utils {
    /// A callable event type that keeps track of callbacks to fire upon being
    /// triggered.
    ///
    /// \tparam E Context object to be passed to parameter-expecting callbacks.
    template <typename E> class event {
    public:
        using simple_fn = void();
        template <typename T> using simple_mem_fn = void (T::*)();
        using ctx_fn = void(E const &);
        template <typename T> using ctx_mem_fn = void (T::*)(E const &);

        /// Connects the event to a function `cb.`
        void connect(simple_fn *cb, void *owner)
        {
            std::scoped_lock l{event_mutex};
            simple_callbacks.insert({owner, cb});
        }

        /// Connects the event to a member-function pointer `cb`, binding it to
        /// `this_arg`.
        template <typename C> void connect(simple_mem_fn<C> cb, C *owner)
        {
            std::scoped_lock l{event_mutex};
            simple_callbacks.insert({owner, std::bind(cb, owner)});
        }

        /// Connects the event to a function `cb`, passing a context object to
        /// it.
        void connect(ctx_fn cb, void *owner)
        {
            std::scoped_lock l{event_mutex};
            ctx_callbacks.insert({owner, cb});
        }

        /// Connects the event to a member-function `cb`, binding it to
        /// `this_arg` and passing a context object to it.
        template <typename C> void connect(ctx_mem_fn<C> cb, C *owner)
        {
            using namespace std::placeholders;
            std::scoped_lock l{event_mutex};
            ctx_callbacks.insert({owner, std::bind(cb, owner, _1)});
        }

        /// Triggers the event, iterating through and calling each callback.
        void operator()(E ctx)
        {
            std::scoped_lock l{event_mutex};
            for (auto const &[_, f] : simple_callbacks) {
                f();
            }
            for (auto const &[_, f] : ctx_callbacks) {
                f(ctx);
            }
        }

        void disconnect(void *owner)
        {
            std::scoped_lock l{event_mutex};
            if (simple_callbacks.contains(owner)) {
                simple_callbacks.erase(owner);
            }
            if (ctx_callbacks.contains(owner)) {
                ctx_callbacks.erase(owner);
            }
        }

    private:
        std::mutex event_mutex;
        /// Stores callbacks that don't take an argument.
        ///
        /// The keys are a form of grouping; When a callback is registered, it
        /// has an owner, which will unregister all its callbacks upon being
        /// detroyed.
        ///
        /// This key (the owner) is usually the `this` pointer of the object,
        /// and is passed again when diconecting callbacks.
        ///
        /// Member-functions are bound before being stored.
        std::unordered_multimap<void *, std::function<simple_fn>>
            simple_callbacks;
        /// Stores callbacks that do take an argument.
        ///
        /// The keys are a form of grouping; When a callback is registered, it
        /// has an owner, which will unregister all its callbacks upon being
        /// detroyed.
        ///
        /// This key (the owner) is usually the `this` pointer of the object,
        /// and is passed again when diconecting callbacks.
        ///
        /// Member-functions are bound before being stored.
        std::unordered_multimap<void *, std::function<ctx_fn>> ctx_callbacks;
    };
} // namespace utils
