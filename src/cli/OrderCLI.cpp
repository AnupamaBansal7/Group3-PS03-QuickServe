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

    out << "\nAvailable Restaurants:\n";
    for (const auto& rest : restaurants) {
        out << "  " << rest.getRestaurantId() << ". " << rest.getName() << "\n";
    }
    out << "\n";

    int restId = InputUtils::readInt("Enter Restaurant ID: ", 1, 9999, in, out);
    const Restaurant* selectedRest = restaurantRepo_->findById(restId);
    if (!selectedRest) {
        out << "Error: Restaurant ID " << restId << " does not exist.\n";
        return;
    }

    out << "\n" << selectedRest->getName() << " Menu:\n";
    out << "  " << std::left << std::setw(8) << "ID"
        << std::setw(24) << "Item"
        << std::setw(14) << "Station"
        << std::setw(8) << "SLA"
        << "\n";

    for (const auto& item : selectedRest->getMenu()) {
        out << "  " << std::left << std::setw(8) << item.getItemId()
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

    auto result = orderService_->createAndRouteOrder(restId, itemIds);
    if (!result.success) {
        out << "\n" << result.errorMessage << "\n";
        return;
    }

    auto order = result.order;
    out << "\nOrder " << order->getOrderId() << " placed (" << order->getRestaurantName() << ")\n";
    out << "Arrival Time: " << TimeUtils::formatTimePoint(order->getArrivalTime()) << "\n\n";
    out << "Item Routing:\n";

    for (const auto& r : result.routingResults) {
        std::string itemName = r.item->getMenuItem().getName();
        std::string stType = stationTypeToString(r.requiredStationType);

        if (r.assignedToStation) {
            out << "  " << std::left << std::setw(22) << itemName << " -> "
                << r.assignedStationId << " (" << stType << ")\n";
        } else {
            out << "  " << std::left << std::setw(22) << itemName << " -> queued for " << stType << "\n";
        }
    }

    out << "\nOrder is now in preparation.\n";
}

}
