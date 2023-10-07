#ifndef JNI_WEBRTC_UTILS_SIGNAL_H_
#define JNI_WEBRTC_UTILS_SIGNAL_H_

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>

/*
 * A signal object may call multiple slots with the same signature. You can connect
 * functions to the signal which will be called when the emit() method on the signal
 * object is invoked. Any argument passed to emit() will be passed to the given
 * functions.
 */
template <typename... Args>
class Signal
{
    public:
        Signal() = default;
        ~Signal() = default;

        /*
         * Copy constructor and assignment create a new signal.
         */
        Signal(Signal const& /*unused*/) {}

        Signal& operator=(Signal const& other)
        {
            if (this != &other) {
                unbindAll();
            }
            return *this;
        }

        /*
         * Move constructor and assignment operator work as expected.
         */
        Signal(Signal && other) noexcept :
            _slots(std::move(other._slots)),
            _current_id(other._current_id) {}

        Signal& operator=(Signal && other) noexcept
        {
            if (this != &other) {
                _slots = std::move(other._slots);
                _current_id = other._current_id;
            }

            return *this;
        }

        /*
         * Connects a std::function to the signal. The returned value can be used to
         * disconnect the function again.
         */
        int bind(std::function<void(Args...)> const& slot) const
        {
            const std::scoped_lock lock(mutex);

            _slots.insert(std::make_pair(++_current_id, slot));

            return _current_id;
        }

        /*
         * Convenience method to connect a member function of an object to this Signal.
         */
        template <typename T>
        int bindMember(const std::weak_ptr<T> & object, void (T::* func)(Args...))
        {
            return bind([=](Args... args) {
                if (std::shared_ptr<T> spt = object.lock()) {
                    ((spt.get())->*func)(args...);
                }
                else {
                    throw std::runtime_error("Bound object is null");
                }
            });
        }

        /*
         * Convenience method to connect a const member function of an object to this Signal.
         */
        template <typename T>
        int bindMember(const std::weak_ptr<T> & object, void (T::* func)(Args...) const) const
        {
            return bind([=](Args... args) {
                if (std::shared_ptr<T> spt = object.lock()) {
                    ((spt.get())->*func)(args...);
                }
                else {
                    throw std::runtime_error("Bound object is null");
                }
            });
        }

        /*
         * Disconnects a previously connected function.
         */
        void unbind(int id) const
        {
            const std::scoped_lock lock(mutex);

            _slots.erase(id);
        }

        /*
         * Disconnects a previously connected member function.
         */
        void unbind(const std::weak_ptr<T> & object) const
        {
            const std::scoped_lock lock(mutex);

            _slots.erase(id);
        }

        /*
         * Disconnects all previously connected functions.
         */
        void unbindAll() const
        {
            const std::scoped_lock lock(mutex);

            _slots.clear();
        }

        /*
         * Calls all connected functions.
         */
        void emit(Args... p)
        {
            for (auto it = _slots.begin(); it != _slots.end();) {
                try {
                    it->second(p...);

                    ++it;
                }
                catch (...) {
                    // Remove slot, e.g. when a bound object has expired.
                    it = _slots.erase(it);
                }
            }
        }

    private:
        mutable std::map<int, std::function<void(Args...)>> _slots;
        mutable int _current_id{ 0 };
        mutable std::mutex mutex;
};

#endif