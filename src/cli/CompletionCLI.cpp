#include "cli/CompletionCLI.h"
#include "utils/InputUtils.h"
#include "utils/TimeUtils.h"
#include <iomanip>
#include <sstream>
#include <cmath>

namespace QuickServe {

CompletionCLI::CompletionCLI(std::shared_ptr<StationService> stationService,
                             std::shared_ptr<IClock> clock)
    : stationService_(stationService), clock_(clock) {}

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
            << (item ? item->getMenuItem().getName() : "N/A");
        if (item && item->getPlannedPrepMinutes() > 0.0) {
            double elapsed = clock_ ? TimeUtils::durationMinutes(st->getBusyStartTime(), clock_->now()) : 0.0;
            out << " [" << std::fixed << std::setprecision(1) << elapsed << "/" << item->getPlannedPrepMinutes() << " min]";
        }
        out << "\n";
    }

    out << "\n";
    std::string line = InputUtils::readTrimmedString("Enter Station ID (or minutes to advance time, e.g. 5 or 10): ", in, out);
    if (line.empty()) {
        return;
    }

    std::string stationId;
    double simulatedMinutes = -1.0;
    std::istringstream iss(line);
    iss >> stationId;
    iss >> simulatedMinutes;

    // Check if user entered a number to advance time
    bool isNumeric = !stationId.empty();
    for (char ch : stationId) {
        if (!std::isdigit(static_cast<unsigned char>(ch)) && ch != '.') {
            isNumeric = false;
            break;
        }
    }

    if (isNumeric && clock_) {
        double mins = std::stod(stationId);
        clock_->advanceMinutes(mins);
        out << "\n[TIME ADVANCED] Kitchen clock advanced by " << mins << " minutes.\n";
        auto completedList = stationService_->updateCookingProgress(clock_->now());
        if (completedList.empty()) {
            out << "No cooking items reached completion yet.\n";
        } else {
            out << "\nItems completed during this time:\n";
            for (const auto& res : completedList) {
                out << ">> " << res.station->getStationId() << ": "
                    << (res.completedItem ? res.completedItem->getOrderId() : "") << " ("
                    << (res.completedItem ? res.completedItem->getMenuItem().getName() : "") << ")"
                    << " - Actual Prep: " << std::fixed << std::setprecision(1) << res.actualPrepMinutes
                    << " min, Target SLA: " << res.targetSlaMinutes
                    << " min -> " << slaStatusToString(res.slaStatus) << "\n";
                if (res.nextAssignedItem) {
                    out << "   * Queued item " << res.nextAssignedItem->getOrderId() << " ("
                        << res.nextAssignedItem->getMenuItem().getName()
                        << ") moved to " << res.station->getStationId() << " [PREPARING]!\n";
                }
            }
        }
        return;
    }

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
