#pragma once
#include <functional>

namespace gl3::engine::events {
    template<typename Owner, typename ...Args>
    class Event {
        friend Owner;

    public:
        using listener_t = std::function<void(Args...)>;
        using container_t = std::list<listener_t>;
        using handle_t = typename container_t::iterator;

        handle_t addListener(listener_t listener) {
            listeners.push_back(listener);
            return --listeners.end();
        }

        void removeListener(handle_t handle) {
            listeners.erase(handle);
        }

    private:
        void invoke(Args ...args) {
            for(container_t unmodifiedCallbacks(listeners); auto &callback: unmodifiedCallbacks) {
                callback(std::forward<Args>(args) ...);
            }
        }

        container_t listeners;
    };
}
