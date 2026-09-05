#include "cli/KitchenStatusCLI.h"
#include <iomanip>

namespace QuickServe {

KitchenStatusCLI::KitchenStatusCLI(std::shared_ptr<ReportService> reportService)
    : reportService_(reportService) {}

void KitchenStatusCLI::display(std::ostream& out) {
    auto statusData = reportService_->getKitchenStatus();

    out << "\n========================================\n";
    out << "KITCHEN STATUS\n";
    out << "==============\n\n";
    out << "Facility: " << statusData.facilityName << "\n\n";

    out << std::left << std::setw(16) << "Station Type"
        << std::setw(8) << "FREE"
        << std::setw(8) << "BUSY"
        << std::setw(8) << "QUEUED"
        << "\n";
    out << "----------------------------------------\n";

    for (const auto& ts : statusData.typeSummaries) {
        out << std::left << std::setw(16) << stationTypeToString(ts.type)
            << std::setw(8) << ts.freeCount
            << std::setw(8) << ts.busyCount
            << std::setw(8) << ts.queuedCount
            << "\n";
    }

    out << "\n";

    // Individual Station Status
    for (const auto& st : statusData.stationDetails) {
        out << st.stationId << " | "
            << std::left << std::setw(10) << stationTypeToString(st.type) << " | "
            << stationStatusToString(st.status);

        if (st.status == StationStatus::BUSY && !st.currentOrderId.empty()) {
            out << " | " << st.currentOrderId;
            if (!st.currentItemName.empty()) {
                out << " (" << st.currentItemName << ")";
            }
        }
        out << "\n";
    }
}

} // namespace QuickServe
