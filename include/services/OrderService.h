#ifndef QUICKSERVE_SERVICES_ORDERSERVICE_H
#define QUICKSERVE_SERVICES_ORDERSERVICE_H

#include "models/Order.h"
#include "repositories/RestaurantRepository.h"
#include "repositories/OrderRepository.h"
#include "services/OrderRoutingService.h"
#include "services/IClock.h"
#include "utils/IdGenerator.h"
#include <vector>
#include <string>
#include <memory>

namespace QuickServe {

struct OrderCreationResult {
    bool success;
    std::string errorMessage;
    std::shared_ptr<Order> order;
    std::vector<RoutingResult> routingResults;
};

class OrderService {
public:
    OrderService(std::shared_ptr<RestaurantRepository> restaurantRepo,
                 std::shared_ptr<OrderRepository> orderRepo,
                 std::shared_ptr<OrderRoutingService> routingService,
                 std::shared_ptr<IClock> clock,
                 std::shared_ptr<IdGenerator> idGenerator);

    OrderCreationResult createAndRouteOrder(int restaurantId, const std::vector<int>& itemIds);

    std::shared_ptr<Order> findOrder(const std::string& orderId) const;
    std::vector<std::shared_ptr<Order>> getAllOrders() const;

private:
    std::shared_ptr<RestaurantRepository> restaurantRepo_;
    std::shared_ptr<OrderRepository> orderRepo_;
    std::shared_ptr<OrderRoutingService> routingService_;
    std::shared_ptr<IClock> clock_;
    std::shared_ptr<IdGenerator> idGenerator_;
};

} // namespace QuickServe

#endif // QUICKSERVE_SERVICES_ORDERSERVICE_H
