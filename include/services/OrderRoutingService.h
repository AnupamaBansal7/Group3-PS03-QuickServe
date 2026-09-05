#ifndef QUICKSERVE_SERVICES_ORDERROUTINGSERVICE_H
#define QUICKSERVE_SERVICES_ORDERROUTINGSERVICE_H

#include "models/OrderItem.h"
#include "models/Order.h"
#include "services/StationService.h"
#include "services/QueueService.h"
#include "services/IClock.h"
#include <memory>
#include <vector>

namespace QuickServe {

struct RoutingResult {
    std::shared_ptr<OrderItem> item;
    bool assignedToStation;
    std::string assignedStationId;
    StationType requiredStationType;
};

class OrderRoutingService {
public:
    OrderRoutingService(std::shared_ptr<StationService> stationService,
                        std::shared_ptr<QueueService> queueService,
                        std::shared_ptr<IClock> clock);

    RoutingResult routeItem(std::shared_ptr<OrderItem> item);
    std::vector<RoutingResult> routeOrder(std::shared_ptr<Order> order);

private:
    std::shared_ptr<StationService> stationService_;
    std::shared_ptr<QueueService> queueService_;
    std::shared_ptr<IClock> clock_;
};

} // namespace QuickServe

#endif // QUICKSERVE_SERVICES_ORDERROUTINGSERVICE_H
