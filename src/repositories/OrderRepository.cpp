#include "repositories/OrderRepository.h"

namespace QuickServe {

void OrderRepository::save(std::shared_ptr<Order> order) {
    if (order) {
        if (orders_.find(order->getOrderId()) == orders_.end()) {
            orderList_.push_back(order);
        }
        orders_[order->getOrderId()] = order;
    }
}

std::shared_ptr<Order> OrderRepository::findById(const std::string& orderId) const {
    auto it = orders_.find(orderId);
    if (it != orders_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Order>> OrderRepository::getAll() const {
    return orderList_;
}

size_t OrderRepository::count() const {
    return orderList_.size();
}

std::vector<std::shared_ptr<OrderItem>> OrderRepository::getCompletedItems() const {
    std::vector<std::shared_ptr<OrderItem>> list;
    for (const auto& ord : orderList_) {
        for (const auto& itm : ord->getOrderItems()) {
            if (itm->getStatus() == OrderItemStatus::COMPLETED) {
                list.push_back(itm);
            }
        }
    }
    return list;
}

std::vector<std::shared_ptr<OrderItem>> OrderRepository::getQueuedItems() const {
    std::vector<std::shared_ptr<OrderItem>> list;
    for (const auto& ord : orderList_) {
        for (const auto& itm : ord->getOrderItems()) {
            if (itm->getStatus() == OrderItemStatus::QUEUED) {
                list.push_back(itm);
            }
        }
    }
    return list;
}

std::vector<std::shared_ptr<OrderItem>> OrderRepository::getPreparingItems() const {
    std::vector<std::shared_ptr<OrderItem>> list;
    for (const auto& ord : orderList_) {
        for (const auto& itm : ord->getOrderItems()) {
            if (itm->getStatus() == OrderItemStatus::PREPARING) {
                list.push_back(itm);
            }
        }
    }
    return list;
}

} // namespace QuickServe
