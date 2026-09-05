#include "services/QueueService.h"

namespace QuickServe {

QueueService::QueueService(std::shared_ptr<IClock> clock,
                           std::shared_ptr<SlaService> slaService,
                           std::shared_ptr<IQueuePriorityStrategy> strategy)
    : clock_(clock),
      slaService_(slaService),
      strategy_(strategy ? strategy : std::make_shared<PeakHourSlaPriorityStrategy>()) {
    queues_[StationType::GRILL] = {};
    queues_[StationType::FRYER] = {};
    queues_[StationType::COLD_PREP] = {};
    queues_[StationType::TANDOOR] = {};
}

void QueueService::setStrategy(std::shared_ptr<IQueuePriorityStrategy> strategy) {
    if (strategy) {
        strategy_ = strategy;
    }
}

void QueueService::enqueue(std::shared_ptr<OrderItem> item) {
    if (!item) return;
    item->setStatus(OrderItemStatus::QUEUED);
    StationType type = item->getMenuItem().getStationType();
    queues_[type].push_back(item);
}

std::shared_ptr<OrderItem> QueueService::getNextItem(StationType type) {
    auto& q = queues_[type];
    if (q.empty()) {
        return nullptr;
    }

    if (strategy_) {
        return strategy_->selectNextItem(q, clock_->now(), *slaService_);
    }

    auto selected = q.front();
    q.pop_front();
    return selected;
}

const std::deque<std::shared_ptr<OrderItem>>& QueueService::getQueue(StationType type) const {
    static const std::deque<std::shared_ptr<OrderItem>> emptyQueue;
    auto it = queues_.find(type);
    if (it != queues_.end()) {
        return it->second;
    }
    return emptyQueue;
}

size_t QueueService::getQueueSize(StationType type) const {
    auto it = queues_.find(type);
    if (it != queues_.end()) {
        return it->second.size();
    }
    return 0;
}

size_t QueueService::getTotalQueueSize() const {
    size_t total = 0;
    for (const auto& pair : queues_) {
        total += pair.second.size();
    }
    return total;
}

bool QueueService::isItemBreaching(const OrderItem& item) const {
    return slaService_->isQueuedItemBreached(item, clock_->now());
}

} // namespace QuickServe
