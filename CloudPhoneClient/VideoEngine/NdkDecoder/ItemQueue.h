/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2023. All rights reserved.
 * Description: Video Engine Client - keep a queue for items that quarantine, save and take
 */

#ifndef ITEMQUEUE_H
#define ITEMQUEUE_H

#include <mutex>
#include <deque>
#include <cstdint>

namespace Vmi {
constexpr uint32_t MAX_ITEM_NUM = 200;

template<typename T>
class ItemQueue {
public:
    /*
     * @brief: construct
     */
    ItemQueue() = default;

    /*
     * @brief: deconstruct
     */
    ~ItemQueue()
    {
        ClearQueue();
    }

    /*
     * @brief: to add specific item at the end of queue
     * @param [in] item
     */
    void PutItem(const T item)
    {
        std::lock_guard<std::mutex> lockGuard(m_lock);
        while (m_deque.size() >= MAX_ITEM_NUM) {
            m_deque.pop_front();
        }
        m_deque.push_back(item);
    }

    /*
     * @brief: to get specific item by index
     * @param [in] index, the item's sequential position in queue
     */
    T GetItemAt(uint32_t index) const
    {
        std::lock_guard<std::mutex> lockGuard(m_lock);
        if (!m_deque.empty() && m_deque.size() > index) {
            return m_deque.at(index);
        }
        return T();
    }

    /*
     * @brief: to get the item number of queue
     */
    uint32_t GetItemNum() const
    {
        std::lock_guard<std::mutex> lockGuard(m_lock);
        return m_deque.size();
    }

    /*
     * @brief: to check the queue empty or not
     */
    bool IsEmpty() const
    {
        std::lock_guard<std::mutex> lockGuard(m_lock);
        return m_deque.empty();
    }

    /*
     * @brief: clear all the items off the queue
     */
    void ClearQueue()
    {
        std::lock_guard<std::mutex> lockGuard(m_lock);
        m_deque.clear();
    }

private:
    mutable std::mutex m_lock = {};
    std::deque<T> m_deque = {};
};
}

#endif // ITEMQUEUE_H
