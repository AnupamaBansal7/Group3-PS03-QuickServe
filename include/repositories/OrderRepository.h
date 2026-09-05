#ifndef QUICKSERVE_REPOSITORIES_ORDERREPOSITORY_H
#define QUICKSERVE_REPOSITORIES_ORDERREPOSITORY_H

#include "models/Order.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace QuickServe {

class OrderRepository {
public:
    void save(std::shared_ptr<Order> order);
    std::shared_ptr<Order> findById(const std::string& orderId) const;
    std::vector<std::shared_ptr<Order>> getAll() const;
    size_t count() const;

    std::vector<std::shared_ptr<OrderItem>> getCompletedItems() const;
    std::vector<std::shared_ptr<OrderItem>> getQueuedItems() const;
    std::vector<std::shared_ptr<OrderItem>> getPreparingItems() const;

private:
    std::map<std::string, std::shared_ptr<Order>> orders_;
    std::vector<std::shared_ptr<Order>> orderList_;
};

} // namespace QuickServe

#endif // QUICKSERVE_REPOSITORIES_ORDERREPOSITORY_H
