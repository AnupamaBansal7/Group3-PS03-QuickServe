#include "models/Order.h"

namespace QuickServe {

Order::Order(const std::string& orderId,
             int restaurantId,
             const std::string& restaurantName,
             std::chrono::system_clock::time_point arrivalTime)
    : orderId_(orderId),
      restaurantId_(restaurantId),
      restaurantName_(restaurantName),
      arrivalTime_(arrivalTime) {}

const std::string& Order::getOrderId() const {
    return orderId_;
}

int Order::getRestaurantId() const {
    return restaurantId_;
}

const std::string& Order::getRestaurantName() const {
    return restaurantName_;
}

std::chrono::system_clock::time_point Order::getArrivalTime() const {
    return arrivalTime_;
}

void Order::addItem(std::shared_ptr<OrderItem> item) {
    orderItems_.push_back(item);
}

const std::vector<std::shared_ptr<OrderItem>>& Order::getOrderItems() const {
    return orderItems_;
}

bool Order::isAllCompleted() const {
    if (orderItems_.empty()) return false;
    for (const auto& item : orderItems_) {
        if (item->getStatus() != OrderItemStatus::COMPLETED) {
            return false;
        }
    }
    return true;
}

} // namespace QuickServe
