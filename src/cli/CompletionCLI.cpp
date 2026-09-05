#include "cli/CompletionCLI.h"
#include "utils/InputUtils.h"
#include <iomanip>
#include <sstream>
#include <cmath>

namespace QuickServe {

CompletionCLI::CompletionCLI(std::shared_ptr<StationService> stationService)
    : stationService_(stationService) {}

void CompletionCLI::display(std::istream& in, std::ostream& out) {
    auto activeStations = stationService_->getActiveStations();
    if (activeStations.empty()) {
        out << "\nNo stations are currently active.\n";
        return;
    }

    out << "\n========================================\n";
    out << "ACTIVE STATIONS\n";
    out << "===============\n\n";

    int idx = 1;
    for (const auto& st : activeStations) {
        auto item = st->getCurrentOrderItem();
        out << idx++ << ". " << st->getStationId() << " | "
            << stationTypeToString(st->getStationType()) << " | "
            << (item ? item->getOrderId() : "N/A") << " | "
            << (item ? item->getMenuItem().getName() : "N/A") << "\n";
    }

    out << "\n";
    std::string line = InputUtils::readTrimmedString("Enter Station ID: ", in, out);
    if (line.empty()) {
        return;
    }

    std::string stationId;
    double simulatedMinutes = -1.0;
    std::istringstream iss(line);
    iss >> stationId;
    iss >> simulatedMinutes;

    auto result = stationService_->completeStationItem(stationId, simulatedMinutes);
    if (!result.success) {
        out << "\n" << result.errorMessage << "\n";
        return;
    }

    out << "\n" << result.station->getStationId() << " completed.\n\n";
    if (std::abs(result.actualPrepMinutes - std::round(result.actualPrepMinutes)) < 0.05) {
        out << "Actual Prep Time: " << static_cast<int>(std::round(result.actualPrepMinutes)) << " minutes\n";
    } else {
        out << "Actual Prep Time: " << std::fixed << std::setprecision(1) << result.actualPrepMinutes << " minutes\n";
    }
    out << "SLA Target: " << result.targetSlaMinutes << " minutes\n\n";
    out << "SLA Status: " << slaStatusToString(result.slaStatus) << "\n\n";
    out << "Station " << result.station->getStationId() << " is now FREE.\n\n";

    out << "Checking " << stationTypeToString(result.station->getStationType()) << " queue...\n\n";
    if (result.nextAssignedItem) {
        out << result.nextAssignedItem->getOrderId() << " assigned to " << result.station->getStationId() << ".\n";
    } else {
        out << "Queue is empty. Station remains FREE.\n";
    }
}

} // namespace QuickServe
