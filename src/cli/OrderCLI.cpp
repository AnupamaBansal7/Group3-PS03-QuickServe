#include "cli/OrderCLI.h"
#include "utils/InputUtils.h"
#include <iomanip>
#include <sstream>

namespace QuickServe {

OrderCLI::OrderCLI(std::shared_ptr<OrderService> orderService,
                   std::shared_ptr<RestaurantRepository> restaurantRepo)
    : orderService_(orderService),
      restaurantRepo_(restaurantRepo) {}

void OrderCLI::display(std::istream& in, std::ostream& out) {
    auto restaurants = restaurantRepo_->getAll();
    if (restaurants.empty()) {
        out << "No restaurants available.\n";
        return;
    }

    out << "\n========================================\n";
    out << "AVAILABLE RESTAURANTS\n";
    out << "=====================\n\n";

    for (const auto& rest : restaurants) {
        out << rest.getRestaurantId() << ". " << rest.getName() << "\n";
    }
    out << "\n";

    int restId = InputUtils::readInt("Enter Restaurant ID: ", 1, 9999, in, out);
    const Restaurant* selectedRest = restaurantRepo_->findById(restId);
    if (!selectedRest) {
        out << "ERROR: Restaurant ID " << restId << " does not exist.\n";
        return;
    }

    out << "\n========================================\n";
    out << selectedRest->getName() << " MENU\n";
    out << "================\n\n";
    out << std::left << std::setw(8) << "ID"
        << std::setw(24) << "ITEM"
        << std::setw(14) << "STATION"
        << std::setw(8) << "SLA"
        << "\n";

    for (const auto& item : selectedRest->getMenu()) {
        out << std::left << std::setw(8) << item.getItemId()
            << std::setw(24) << item.getName()
            << std::setw(14) << stationTypeToString(item.getStationType())
            << (std::to_string(item.getSlaMinutes()) + " min")
            << "\n";
    }

    out << "\nEnter item IDs separated by spaces:\n";
    auto itemIds = InputUtils::readIntList("> ", in, out);
    if (itemIds.empty()) {
        out << "Order cancelled.\n";
        return;
    }

    // Call service to validate, create, and route order
    auto result = orderService_->createAndRouteOrder(restId, itemIds);

    if (!result.success) {
        out << "\n" << result.errorMessage << "\n";
        return;
    }

    auto order = result.order;
    out << "\n========================================\n";
    out << "ORDER CONFIRMATION\n";
    out << "==================\n\n";
    out << "Order ID: " << order->getOrderId() << "\n";
    out << "Restaurant: " << order->getRestaurantName() << "\n";
    out << "Arrival Time: " << TimeUtils::formatTimePoint(order->getArrivalTime()) << "\n\n";
    out << "Routing order...\n\n";

    for (const auto& r : result.routingResults) {
        std::string itemName = r.item->getMenuItem().getName();
        std::string stType = stationTypeToString(r.requiredStationType);

        if (r.assignedToStation) {
            out << std::left << std::setw(16) << itemName << " -> "
                << r.assignedStationId << " [" << stType << "]\n";
        } else {
            out << itemName << " added to " << stType << " queue.\n";
        }
    }

    out << "\nYour order is being prepared.\n";
}

} // namespace QuickServe
