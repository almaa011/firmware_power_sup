/**
 * @file triple_buffer.h
 * @author Jacob Bunzel (bunze002@umn.edu)
 * @brief Defines triple buffer class.
 * @date 2022-02-04
 */

#pragma once
#include <array>
#if STM32G0B1xx

namespace umnsvp {
namespace triple_buffer {

/**
 * @brief Lock-free triple buffer class.
 *
 * Example Usage:
    ........................
    // In an interrupt
    buffer.push(data);
    ........................
    // In the application
    if(buffer.pop()){
        data = buffer.output();
        // Do stuffs with the data!
    }
    ........................
 *
 * @tparam T Buffered type. Must have a default constructor.
 */
template <typename T>
class TripleBuffer {
   public:
    /**
     * @brief Construct a new Triple Buffer.
     *
     * @param init_val Initial value for buffer elements. Defaults to @ref T
     * default constructor. Thus, T must have a default constructor.
     */
    TripleBuffer(T init_val = T()) : consuming(0), transfer(1), producing(2) {
        stale = true;

        buffer = {init_val, init_val, init_val};
    }

    /**
     * @brief Produce a new piece of data in the triple buffer.
     *
     * @param element Data element to be added to the buffer.
     * @return true Push was successful. Currently, it always is as it
     * prioritizes getting new data into the buffer.
     * @return false Push was unsuccessful. Currently, never false, see return
     * true documentation above.
     */
    bool push(T& element) {
        __disable_irq();

        buffer[producing] = element;

        uint8_t temp = producing;
        producing = transfer;
        transfer = temp;

        stale = false;
        __enable_irq();
        return true;
    }

    /**
     * @brief Remove the current consumable element for a new element if present
     * in the buffer.
     *
     * @return true The current element was removed and replaced with a new
     * element.
     * @return false There was no new element and the current element will still
     * be outputted.
     */
    bool pop() {
        __disable_irq();
        if (!stale) {
            uint8_t temp = transfer;
            transfer = consuming;
            consuming = temp;

            stale = true;
            __enable_irq();
            return true;
        } else {
            __enable_irq();
            return false;
        }
    }

    /**
     * @brief Get the current consummable element.
     *
     * Fresh data is not available here until a pop() that returns true occurs.
     * See the example in the class docs above.
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
    TripleBuffer& operator=(const TripleBuffer&) = delete;
    TripleBuffer& operator=(const TripleBuffer&) volatile = delete;
    TripleBuffer(const TripleBuffer&) = delete;
    /**
     * @}
     */

   private:
    /**
     * @brief The buffer with three elements storing the actual data.
     */
    std::array<T, 3> buffer;

    /**
     * @brief Whether the transfer element has stale data.
     *
     * When a push occurs, the new data is transferred into the transfer
     * element. This new data is not stale. Once it is popped, the old data
     * from the consuming element moves into the transfer element. This is
     * stale data.
     */
    bool stale;

    /**
     * @brief Index to element in @ref buffer that is comsumable data.
     */
    uint8_t consuming;

    /**
     * @brief Index to element in @ref buffer that is not being consumed or
     * produced, but will be transferred to one soon.
     */
    uint8_t transfer;

    /**
     * @brief Index to element in @ref buffer that is get data produced into it.
     */
    uint8_t producing;
};
}  // namespace triple_buffer
}  // namespace umnsvp
#else
#include <atomic>

namespace umnsvp {
namespace triple_buffer {

/**
 * @brief Lock-free triple buffer class.
 *
 * Example Usage:
    ........................
    // In an interrupt
    buffer.push(data);
    ........................
    // In the application
    if(buffer.pop()){
        data = buffer.output();
        // Do stuffs with the data!
    }
    ........................
 *
 * @tparam T Buffered type. Must have a default constructor.
 */
template <typename T>
class TripleBuffer {
   public:
    /**
     * @brief Construct a new Triple Buffer.
     *
     * @param init_val Initial value for buffer elements. Defaults to @ref T
     * default constructor. Thus, T must have a default constructor.
     */
    TripleBuffer(T init_val = T()) : consuming(0), transfer(1), producing(2) {
        stale = true;

        buffer = {init_val, init_val, init_val};
    }

    /**
     * @brief Produce a new piece of data in the triple buffer.
     *
     * @param element Data element to be added to the buffer.
     * @return true Push was successful. Currently, it always is as it
     * prioritizes getting new data into the buffer.
     * @return false Push was unsuccessful. Currently, never false, see return
     * true documentation above.
     */
    bool push(T& element) {
        buffer[producing.load()] = element;

        producing = transfer.exchange(producing);

        stale = false;

        return true;
    }

    /**
     * @brief Remove the current consumable element for a new element if present
     * in the buffer.
     *
     * @return true The current element was removed and replaced with a new
     * element.
     * @return false There was no new element and the current element will still
     * be outputted.
     */
    bool pop() {
        if (!stale) {
            transfer = consuming.exchange(transfer);
            stale = true;
            return true;
        } else {
            return false;
        }
    }

    /**
     * @brief Get the current consummable element.
     *
     * Fresh data is not available here until a pop() that returns true occurs.
     * See the example in the class docs above.
     *
     * @return T Current consumable element.
     */
    T output() const {
        return buffer[consuming.load()];
    }

    /**
     * @brief Delete default assignment operator and copy constructor as using
     * them on this class would both not make sense and would not compile.
     *
     * @{
     */
    TripleBuffer& operator=(const TripleBuffer&) = delete;
    TripleBuffer& operator=(const TripleBuffer&) volatile = delete;
    TripleBuffer(const TripleBuffer&) = delete;
    /**
     * @}
     */

   private:
    /**
     * @brief The buffer with three elements storing the actual data.
     */
    std::array<T, 3> buffer;

    /**
     * @brief Whether the transfer element has stale data.
     *
     * When a push occurs, the new data is transferred into the transfer
     * element. This new data is not stale. Once it is popped, the old data
     * from the consuming element moves into the transfer element. This is
     * stale data.
     */
    std::atomic<bool> stale;

    /**
     * @brief Index to element in @ref buffer that is comsumable data.
     */
    std::atomic<uint8_t> consuming;

    /**
     * @brief Index to element in @ref buffer that is not being consumed or
     * produced, but will be transferred to one soon.
     */
    std::atomic<uint8_t> transfer;

    /**
     * @brief Index to element in @ref buffer that is get data produced into it.
     */
    std::atomic<uint8_t> producing;
};
}  // namespace triple_buffer
}  // namespace umnsvp
#endif
