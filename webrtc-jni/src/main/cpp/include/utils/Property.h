#ifndef JNI_WEBRTC_UTILS_PROPERTY_H_
#define JNI_WEBRTC_UTILS_PROPERTY_H_

#include "utils/Signal.h"

#include <iostream>

/*
 * A Property encpsulates a value and may inform subscribers on any changes
 * applied to this value.
 */
template <typename T>
class Property
{
    public:
        typedef T value_type;

        /*
         * Properties for built-in types are automatically initialized to 0.
         * See template spezialisations at the bottom.
         */
        Property()
        {}

        Property(T const& val) :
            value_(val)
        {}

        Property(T && val) :
            value_(std::move(val))
        {}

        Property(Property<T> const& to_copy) :
            value_(to_copy.value_)
        {}

        Property(Property<T> && to_copy) :
            value_(std::move(to_copy.value_))
        {}

        /*
         * Returns a Signal which is fired when the internal value has been changed.
         * The new value is passed as parameter.
         */
        virtual Signal<T> const& signal() const
        {
            return signal_;
        }

        /*
         * Sets the Property to a new value.
         */
        virtual void set(T const& value)
        {
            if (value != value_) {
                value_ = value;
                signal_.emit(value_);
            }
        }

        /*
         * Returns the internal value.
         */
        virtual T const& get() const
        {
            return value_;
        }

        /*
         * If there are any subscribers connected to this Property, they won't be notified
         * of any further changes.
         */
        virtual void disconnect_auditors()
        {
            signal_.unbindAll();
        }

        /*
         * Assigns the value of another Property.
         */
        virtual Property<T>& operator=(Property<T> const& rhs)
        {
            set(rhs.value_);
            return *this;
        }

        /*
         * Assigns a new value to this Property
         */
        virtual Property<T>& operator=(T const& rhs)
        {
            set(rhs);
            return *this;
        }

        /*
         * Compares the values of two Properties.
         */
        bool operator==(Property<T> const& rhs) const
        {
            return Property<T>::get() == rhs.get();
        }

        bool operator!=(Property<T> const& rhs) const
        {
            return Property<T>::get() != rhs.get();
        }

        /*
         * Compares the value of this Property to another value.
         */
        bool operator==(T const& rhs) const
        {
            return Property<T>::get() == rhs;
        }

        bool operator!=(T const& rhs) const
        {
            return Property<T>::get() != rhs;
        }

        /*
         * Returns the value of this Property.
         */
        T const& operator()() const
        {
            return Property<T>::get();
        }

    private:
        Signal<T> signal_;
        T value_;
};


/*
 * Specialization for built-in default contructors.
 */
template<> inline Property<double>::Property() : value_(0.0) {}
template<> inline Property<float>::Property() : value_(0.f) {}
template<> inline Property<short>::Property() : value_(0) {}
template<> inline Property<int>::Property() : value_(0) {}
template<> inline Property<char>::Property() : value_(0) {}
template<> inline Property<unsigned>::Property() : value_(0) {}
template<> inline Property<bool>::Property() : value_(false) {}


/*
 * Stream operators.
 */
template<typename T>
std::ostream& operator<<(std::ostream & out_stream, Property<T> const& val) {
    out_stream << val.get();
    return out_stream;
}

template<typename T>
std::istream& operator>>(std::istream & in_stream, Property<T> & val) {
    T tmp;
    in_stream >> tmp;
    val.set(tmp);
    return in_stream;
}

#endif
