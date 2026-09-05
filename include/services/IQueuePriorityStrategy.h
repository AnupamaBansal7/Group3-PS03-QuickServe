#ifndef QUICKSERVE_SERVICES_IQUEUEPRIORITYSTRATEGY_H
#define QUICKSERVE_SERVICES_IQUEUEPRIORITYSTRATEGY_H

#include "models/OrderItem.h"
#include <deque>
#include <memory>
#include <chrono>

namespace QuickServe {

class SlaService;

class IQueuePriorityStrategy {
public:
    virtual ~IQueuePriorityStrategy() = default;

    virtual std::shared_ptr<OrderItem> selectNextItem(
        std::deque<std::shared_ptr<OrderItem>>& queue,
        std::chrono::system_clock::time_point currentTime,
        const SlaService& slaService) = 0;
};

class PeakHourSlaPriorityStrategy : public IQueuePriorityStrategy {
public:
    std::shared_ptr<OrderItem> selectNextItem(
        std::deque<std::shared_ptr<OrderItem>>& queue,
        std::chrono::system_clock::time_point currentTime,
        const SlaService& slaService) override;
};

class FifoPriorityStrategy : public IQueuePriorityStrategy {
public:
    std::shared_ptr<OrderItem> selectNextItem(
        std::deque<std::shared_ptr<OrderItem>>& queue,
        std::chrono::system_clock::time_point currentTime,
        const SlaService& slaService) override;
};

}

#endif
