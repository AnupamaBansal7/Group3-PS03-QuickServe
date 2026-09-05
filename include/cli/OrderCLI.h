#ifndef QUICKSERVE_CLI_ORDERCLI_H
#define QUICKSERVE_CLI_ORDERCLI_H

#include "services/OrderService.h"
#include "repositories/RestaurantRepository.h"
#include "utils/TimeUtils.h"
#include <memory>
#include <iostream>

namespace QuickServe {

class OrderCLI {
public:
    OrderCLI(std::shared_ptr<OrderService> orderService,
             std::shared_ptr<RestaurantRepository> restaurantRepo);

    void display(std::istream& in = std::cin, std::ostream& out = std::cout);

private:
    std::shared_ptr<OrderService> orderService_;
    std::shared_ptr<RestaurantRepository> restaurantRepo_;
};

} // namespace QuickServe

#endif // QUICKSERVE_CLI_ORDERCLI_H
