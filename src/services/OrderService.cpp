#include "services/OrderService.h"
#include <sstream>

namespace QuickServe {

OrderService::OrderService(std::shared_ptr<RestaurantRepository> restaurantRepo,
                           std::shared_ptr<OrderRepository> orderRepo,
                           std::shared_ptr<OrderRoutingService> routingService,
                           std::shared_ptr<IClock> clock,
                           std::shared_ptr<IdGenerator> idGenerator)
    : restaurantRepo_(restaurantRepo),
      orderRepo_(orderRepo),
      routingService_(routingService),
      clock_(clock),
      idGenerator_(idGenerator) {}

OrderCreationResult OrderService::createAndRouteOrder(int restaurantId, const std::vector<int>& itemIds) {
    OrderCreationResult result;
    result.success = false;
    result.order = nullptr;

    const Restaurant* restaurant = restaurantRepo_->findById(restaurantId);
    if (!restaurant) {
        std::ostringstream oss;
        oss << "ERROR: Restaurant ID " << restaurantId << " does not exist.";
        result.errorMessage = oss.str();
        return result;
    }

    if (itemIds.empty()) {
        result.errorMessage = "ERROR: Order must contain at least one item.";
        return result;
    }

    // Validate every item ID against selected restaurant's menu
    for (int itemId : itemIds) {
        if (!restaurant->hasMenuItem(itemId)) {
            std::ostringstream oss;
            oss << "ERROR: Item ID " << itemId << " does not exist in "
                << restaurant->getName() << " menu.\nOrder rejected.";
            result.errorMessage = oss.str();
            return result;
        }
    }

    // All items valid: create order
    std::string orderId = idGenerator_->nextOrderId();
    auto arrivalTime = clock_->now();

    auto order = std::make_shared<Order>(orderId, restaurantId, restaurant->getName(), arrivalTime);

    int seq = 1;
    for (int itemId : itemIds) {
        const MenuItem* mi = restaurant->findMenuItem(itemId);
        std::string orderItemId = idGenerator_->nextOrderItemId(orderId, seq++);
        auto orderItem = std::make_shared<OrderItem>(orderItemId, orderId, *mi, arrivalTime);
        order->addItem(orderItem);
    }

    // Save order
    orderRepo_->save(order);

    // Route order items
    result.routingResults = routingService_->routeOrder(order);
    result.order = order;
    result.success = true;

    return result;
}

std::shared_ptr<Order> OrderService::findOrder(const std::string& orderId) const {
    return orderRepo_->findById(orderId);
}

std::vector<std::shared_ptr<Order>> OrderService::getAllOrders() const {
    return orderRepo_->getAll();
}

} // namespace QuickServe
