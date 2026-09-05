#include "services/OrderRoutingService.h"

namespace QuickServe {

OrderRoutingService::OrderRoutingService(std::shared_ptr<StationService> stationService,
                                         std::shared_ptr<QueueService> queueService,
                                         std::shared_ptr<IClock> clock)
    : stationService_(stationService),
      queueService_(queueService),
      clock_(clock) {}

RoutingResult OrderRoutingService::routeItem(std::shared_ptr<OrderItem> item) {
    RoutingResult result;
    result.item = item;
    result.assignedToStation = false;
    result.assignedStationId = "";
    if (!item) return result;

    StationType reqType = item->getMenuItem().getStationType();
    result.requiredStationType = reqType;

    auto freeStation = stationService_->findFreeStation(reqType);
    if (freeStation) {
        stationService_->assignItem(freeStation, item);
        result.assignedToStation = true;
        result.assignedStationId = freeStation->getStationId();
    } else {
        queueService_->enqueue(item);
        result.assignedToStation = false;
    }

    return result;
}

std::vector<RoutingResult> OrderRoutingService::routeOrder(std::shared_ptr<Order> order) {
    std::vector<RoutingResult> results;
    if (!order) return results;

    for (const auto& item : order->getOrderItems()) {
        results.push_back(routeItem(item));
    }
    return results;
}

} // namespace QuickServe
