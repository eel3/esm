/* ********************************************************************** */
/**
 * @brief   ESM: Inter-thread communication mailbox (sample && test application).
 * @author  eel3
 * @date    2017-10-18
 */
/* ********************************************************************** */

#ifndef MAILBOX_H_INCLUDED
#define MAILBOX_H_INCLUDED

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

/* ---------------------------------------------------------------------- */
/* Template Classes */
/* ---------------------------------------------------------------------- */

/** Inter-thread communication mailbox class. */
template <typename T>
class Mailbox {
private:
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::queue<T> m_queue;

public:
    void push(const T& val) {
        std::lock_guard<std::mutex> lck(m_mutex);
        m_queue.push(val);
        m_cond.notify_one();
    }

    bool pop(T& val) {
        std::lock_guard<std::mutex> lck(m_mutex);
        if (m_queue.empty()) {
            return false;
        };
        val = m_queue.front();
        m_queue.pop();
        return true;
    }
};

#endif /* ndef MAILBOX_H_INCLUDED */
