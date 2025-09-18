/**
 * @file circular_buffer.h
 * @author Brett Duncan (dunca384@umn.edu)
 * @brief Defines circular buffer class.
 * @date 2022-03-29
 */

#pragma once
#include <array>

#include "can_packet.h"
#if STM32G0B1xx

namespace umnsvp {
namespace circular_buffer {

/**
 * @brief Circular Buffer class. Not threadsafe, incorrect usage may result in
 * data loss or coruption.
 *
 * Example Usage:
   ........................
   // In your main function
   buffer.push(data);
   ........................
   // In an interrupt - pop data
   if (buffer.pop()) {
       data = buffer.output();
   }
   // or to access the first item without modifying the data in the buffer:
   if (buffer.peek()) {
       data = buffer.output();
   }
   ........................
 * @tparam T Buffered type. Must have a default constructor.
 */
template <typename T, std::size_t size>
class CircularBuffer {
   public:
    /**
     * @brief Construct a new Triple Buffer.
     *
     * @param init_val Initial value for buffer elements. Defaults to @ref T
     * default constructor. Thus, T must have a default constructor.
     */
    CircularBuffer(T init_val = T()) {
        for (auto& item : buffer) {
            item = init_val;
        }
    }

    /**
     * @brief Push a new piece of data into the circular buffer. This method
     * will overwrite the last element in the buffer if the buffer is already
     * full.
     *
     * @param element Data element to be added to the buffer.
     * @return true Buffer is full.
     * @return false Buffer is not full.
     */
    bool push(T& element) {
        __disable_irq();
        // get the value of front
        uint8_t f = front;
        buffer[f] = element;
        f++;
        if (f == buffer.size()) {
            f = 0;
        }
        if ((f == (back - 1)) || ((f == buffer.size() - 1) && (back == 0))) {
            // the buffer is full, exit without updating the value
            // of front
            __enable_irq();
            return true;
        }
        // set the value of front
        front = f;
        __enable_irq();
        return false;
    }

    /**
     * @brief Remove and output current element if available.
     *
     * @return true New data was available and will be outputed.
     * @return false There was no new data and the current data will still
     * be outputted.
     */
    bool pop() {
        __disable_irq();
        uint8_t b = back;
        if (b != front) {
            consuming = b;
            b++;
            if (b == buffer.size()) {
                b = 0;
            }
            back = b;
            __enable_irq();
            return true;
        }
        __enable_irq();
        return false;
    }

    /**
     * @brief Output the current element if available.
     *
     * @return true New data was available and will be outputed.
     * @return false There was no new data and the current data will still
     * be outputted.
     */
    bool peek() {
        __disable_irq();
        uint8_t b = back;
        if (b != front) {
            consuming = b;
            __enable_irq();
            return true;
        }
        __enable_irq();
        return false;
    }

    /**
     * @brief Get the current consummable element.
     *
     * Fresh data is not available here until a pop() or peek() that returns
     * true occurs. See the example in the class docs above.
     *
     * @return T Current consumable element.
     */
    T output() const {
        return buffer[consuming];
    }

    /**
     * @brief Delete default assignment operator and copy constructor as using
     * them on this class would both not make sense and would not compile.
     *
     * @{
     */
    CircularBuffer& operator=(const CircularBuffer&) = delete;
    CircularBuffer& operator=(const CircularBuffer&) volatile = delete;
    CircularBuffer(const CircularBuffer&) = delete;
    /**
     * @}
     */

   private:
    std::array<T, size> buffer;

    uint8_t front = 0;
    uint8_t back = 0;
    uint8_t consuming = 0;
};
}  // namespace circular_buffer
}  // namespace umnsvp
#else
#include <atomic>
namespace umnsvp {
namespace circular_buffer {

/**
 * @brief Circular Buffer class. Not threadsafe, incorrect usage may result in
 * data loss or coruption.
 *
 * Example Usage:
   ........................
   // In your main function
   buffer.push(data);
   ........................
   // In an interrupt - pop data
   if (buffer.pop()) {
       data = buffer.output();
   }
   // or to access the first item without modifying the data in the buffer:
   if (buffer.peek()) {
       data = buffer.output();
   }
   ........................
 * @tparam T Buffered type. Must have a default constructor.
 */
template <typename T, std::size_t size>
class CircularBuffer {
   public:
    /**
     * @brief Construct a new Triple Buffer.
     *
     * @param init_val Initial value for buffer elements. Defaults to @ref T
     * default constructor. Thus, T must have a default constructor.
     */
    CircularBuffer(T init_val = T()) {
        for (auto& item : buffer) {
            item = init_val;
        }
    }

    /**
     * @brief Push a new piece of data into the circular buffer. This method
     * will overwrite the last element in the buffer if the buffer is already
     * full.
     *
     * @param element Data element to be added to the buffer.
     * @return true Buffer is full.
     * @return false Buffer is not full.
     */
    bool push(T& element) {
        // get the value of front
        uint8_t f = front.load();
        buffer[f] = element;
        f++;
        if (f == buffer.size()) {
            f = 0;
        }
        if ((f == (back.load() - 1)) ||
            ((f == buffer.size() - 1) && (back.load() == 0))) {
            // the buffer is full, exit without updating the value
            // of front
            return true;
        }
        // set the value of front
        front.exchange(f);
        return false;
    }

    /**
     * @brief Remove and output current element if available.
     *
     * @return true New data was available and will be outputed.
     * @return false There was no new data and the current data will still
     * be outputted.
     */
    bool pop() {
        uint8_t b = back.load();
        if (b != front.load()) {
            consuming.exchange(b);
            b++;
            if (b == buffer.size()) {
                b = 0;
            }
            back.exchange(b);
            return true;
        }
        return false;
    }

    /**
     * @brief Output the current element if available.
     *
     * @return true New data was available and will be outputed.
     * @return false There was no new data and the current data will still
     * be outputted.
     */
    bool peek() {
        uint8_t b = back.load();
        if (b != front.load()) {
            consuming.exchange(b);
            return true;
        }
        return false;
    }

    /**
     * @brief Get the current consummable element.
     *
     * Fresh data is not available here until a pop() or peek() that returns
     * true occurs. See the example in the class docs above.
     *
     * @return T Current consumable element.
     */
    T output() const {
        uint8_t index = consuming.load();
        return buffer[index];
    }

    /**
     * @brief Delete default assignment operator and copy constructor as using
     * them on this class would both not make sense and would not compile.
     *
     * @{
     */
    CircularBuffer& operator=(const CircularBuffer&) = delete;
    CircularBuffer& operator=(const CircularBuffer&) volatile = delete;
    CircularBuffer(const CircularBuffer&) = delete;
    /**
     * @}
     */

   private:
    std::array<T, size> buffer;

    std::atomic<uint8_t> front = 0;
    std::atomic<uint8_t> back = 0;
    std::atomic<uint8_t> consuming = 0;
};
}  // namespace circular_buffer
}  // namespace umnsvp
#endif