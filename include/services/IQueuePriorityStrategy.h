#ifndef QUICKSERVE_SERVICES_IQUEUEPRIORITYSTRATEGY_H
#define QUICKSERVE_SERVICES_IQUEUEPRIORITYSTRATEGY_H

#include "models/OrderItem.h"
#include <deque>
#include <memory>
#include <chrono>

namespace QuickServe {

class SlaService;

/**
 * @brief Strategy interface for queue item dispatch prioritization.
 */
class IQueuePriorityStrategy {
public:
    virtual ~IQueuePriorityStrategy() = default;

    virtual std::shared_ptr<OrderItem> selectNextItem(
        std::deque<std::shared_ptr<OrderItem>>& queue,
        std::chrono::system_clock::time_point currentTime,
        const SlaService& slaService) = 0;
};

/**
 * @brief Peak-hour SLA prioritization strategy:
 * Items are served in arrival order (FIFO), EXCEPT any item already breaching
 * its target preparation SLA in the queue jumps ahead of normal items.
 * Relative arrival order among multiple breaching items is preserved.
 */
class PeakHourSlaPriorityStrategy : public IQueuePriorityStrategy {
public:
    std::shared_ptr<OrderItem> selectNextItem(
        std::deque<std::shared_ptr<OrderItem>>& queue,
        std::chrono::system_clock::time_point currentTime,
        const SlaService& slaService) override;
};

/**
 * @brief Standard FIFO prioritization strategy without SLA leapfrogging.
 */
class FifoPriorityStrategy : public IQueuePriorityStrategy {
public:
    std::shared_ptr<OrderItem> selectNextItem(
        std::deque<std::shared_ptr<OrderItem>>& queue,
        std::chrono::system_clock::time_point currentTime,
        const SlaService& slaService) override;
};

} // namespace QuickServe

#endif // QUICKSERVE_SERVICES_IQUEUEPRIORITYSTRATEGY_H
