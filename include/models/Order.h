#ifndef QUICKSERVE_MODELS_ORDER_H
#define QUICKSERVE_MODELS_ORDER_H

#include "models/OrderItem.h"
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace QuickServe {

class Order {
public:
    Order(const std::string& orderId,
          int restaurantId,
          const std::string& restaurantName,
          std::chrono::system_clock::time_point arrivalTime);

    const std::string& getOrderId() const;
    int getRestaurantId() const;
    const std::string& getRestaurantName() const;
    std::chrono::system_clock::time_point getArrivalTime() const;

    void addItem(std::shared_ptr<OrderItem> item);
    const std::vector<std::shared_ptr<OrderItem>>& getOrderItems() const;

    bool isAllCompleted() const;

private:
    std::string orderId_;
    int restaurantId_;
    std::string restaurantName_;
    std::chrono::system_clock::time_point arrivalTime_;
    std::vector<std::shared_ptr<OrderItem>> orderItems_;
};

} // namespace QuickServe

#endif // QUICKSERVE_MODELS_ORDER_H
