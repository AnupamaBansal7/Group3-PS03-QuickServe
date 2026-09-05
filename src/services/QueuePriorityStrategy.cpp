#include "services/IQueuePriorityStrategy.h"
#include "services/SlaService.h"

namespace QuickServe {

std::shared_ptr<OrderItem> PeakHourSlaPriorityStrategy::selectNextItem(
    std::deque<std::shared_ptr<OrderItem>>& queue,
    std::chrono::system_clock::time_point currentTime,
    const SlaService& slaService) {
    if (queue.empty()) {
        return nullptr;
    }

    // 1. Check for first SLA-breaching item in arrival order
    for (auto it = queue.begin(); it != queue.end(); ++it) {
        if (slaService.isQueuedItemBreached(**it, currentTime)) {
            auto selected = *it;
            queue.erase(it);
            return selected;
        }
    }

    // 2. Otherwise, pop normal item in FIFO arrival order
    auto selected = queue.front();
    queue.pop_front();
    return selected;
}

std::shared_ptr<OrderItem> FifoPriorityStrategy::selectNextItem(
    std::deque<std::shared_ptr<OrderItem>>& queue,
    std::chrono::system_clock::time_point /*currentTime*/,
    const SlaService& /*slaService*/) {
    if (queue.empty()) {
        return nullptr;
    }
    auto selected = queue.front();
    queue.pop_front();
    return selected;
}

} // namespace QuickServe
