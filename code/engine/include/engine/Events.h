#pragma once
#include <functional>

/**
 * @file Events.h
 * @brief Defines the Event class template for managing engine related listener callbacks.
 */

namespace gl3::engine::events
{
    /**
     * @brief A template class representing an event with listeners.
     *
     * This class allows an owner to add and remove listeners (callbacks) and
     * invoke them with specified arguments.
     *
     * @tparam Owner The class that owns this Event.
     * @tparam Args The argument types passed to the listeners when the event is invoked.
     */
    template <typename Owner, typename... Args>
    class Event
    {
        friend Owner;

    public:
        /**
         * @brief The listener type (a function taking Args...).
         */
        using listener_t = std::function<void(Args...)>;

        /**
         * @brief The container type used to store listeners.
         */
        using container_t = std::list<listener_t>;

        /**
         * @brief A handle type for managing added listeners.
         *
         * This is an iterator pointing to the listener in the container.
         */
        using handle_t = typename container_t::iterator;

        /**
         * @brief Adds a listener (callback) to the event.
         *
         * @param listener The callback function to add.
         * @return handle_t A handle that can be used to remove the listener later.
         */
        handle_t addListener(listener_t listener)
        {
            listeners.push_back(listener);
            return --listeners.end();
        }

        /**
         * @brief Removes a previously added listener.
         *
         * @param handle The handle returned by addListener.
         */
        void removeListener(handle_t handle)
        {
            listeners.erase(handle);
        }

    private:
        /**
         * @brief Invokes all registered listeners with the provided arguments.
         *
         * @param args The arguments to pass to each listener.
         */
        void invoke(Args... args)
        {
            // Make a copy to allow safe modification of listeners during iteration.
            for (container_t unmodifiedCallbacks(listeners); auto& callback : unmodifiedCallbacks)
            {
                callback(std::forward<Args>(args)...);
            }
        }

        /**
         * @brief Container storing all active listeners.
         */
        container_t listeners;
    };
} // namespace gl3::engine::events
