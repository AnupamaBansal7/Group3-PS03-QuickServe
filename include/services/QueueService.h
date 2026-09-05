#ifndef QUICKSERVE_SERVICES_QUEUESERVICE_H
#define QUICKSERVE_SERVICES_QUEUESERVICE_H

#include "models/OrderItem.h"
#include "models/Enums.h"
#include "services/IClock.h"
#include "services/SlaService.h"
#include "services/IQueuePriorityStrategy.h"
#include <deque>
#include <map>
#include <memory>
#include <vector>

namespace QuickServe {

class QueueService {
public:
    QueueService(std::shared_ptr<IClock> clock,
                 std::shared_ptr<SlaService> slaService,
                 std::shared_ptr<IQueuePriorityStrategy> strategy = nullptr);

    void setStrategy(std::shared_ptr<IQueuePriorityStrategy> strategy);

    void enqueue(std::shared_ptr<OrderItem> item);

    std::shared_ptr<OrderItem> getNextItem(StationType type);

    const std::deque<std::shared_ptr<OrderItem>>& getQueue(StationType type) const;
    size_t getQueueSize(StationType type) const;
    size_t getTotalQueueSize() const;

    bool isItemBreaching(const OrderItem& item) const;

private:
    std::shared_ptr<IClock> clock_;
    std::shared_ptr<SlaService> slaService_;
    std::shared_ptr<IQueuePriorityStrategy> strategy_;
    std::map<StationType, std::deque<std::shared_ptr<OrderItem>>> queues_;
};

} // namespace QuickServe

#endif // QUICKSERVE_SERVICES_QUEUESERVICE_H
