#include "cli/KitchenStatusCLI.h"
#include <iomanip>

namespace QuickServe {

KitchenStatusCLI::KitchenStatusCLI(std::shared_ptr<ReportService> reportService)
    : reportService_(reportService) {}

void KitchenStatusCLI::display(std::ostream& out) {
    auto statusData = reportService_->getKitchenStatus();

    out << "\nKitchen Status (" << statusData.facilityName << ")\n";
    out << "Current Time: " << statusData.currentTimeStr << "\n\n";

    out << "Station Availability:\n";
    out << "  " << std::left << std::setw(16) << "Station Type"
        << std::setw(8) << "Free"
        << std::setw(8) << "Busy"
        << std::setw(8) << "Queued"
        << "\n";

    for (const auto& ts : statusData.typeSummaries) {
        out << "  " << std::left << std::setw(16) << stationTypeToString(ts.type)
            << std::setw(8) << ts.freeCount
            << std::setw(8) << ts.busyCount
            << std::setw(8) << ts.queuedCount
            << "\n";
    }

    out << "\nPhysical Stations:\n";
    for (const auto& st : statusData.stationDetails) {
        out << "  " << std::left << std::setw(8) << st.stationId
            << std::setw(12) << stationTypeToString(st.type)
            << std::setw(8) << stationStatusToString(st.status);

        if (st.status == StationStatus::BUSY && !st.currentOrderId.empty()) {
            out << "  " << st.currentOrderId;
            if (!st.currentItemName.empty()) {
                out << " (" << st.currentItemName << ")";
            }
            if (st.plannedMinutes > 0.0) {
                out << " [" << std::fixed << std::setprecision(1) << st.elapsedMinutes
                    << "/" << st.plannedMinutes << " min]";
            }
        }
        out << "\n";
    }

    out << "\nQueued Items:\n";
    if (statusData.queuedItems.empty()) {
        out << "  No items currently in queue.\n";
    } else {
        out << "  " << std::left << std::setw(12) << "Order"
            << std::setw(22) << "Item"
            << std::setw(14) << "Station"
            << std::setw(12) << "Target SLA"
            << std::setw(12) << "Wait Time"
            << std::setw(14) << "Priority"
            << "\n";
        for (const auto& q : statusData.queuedItems) {
            std::ostringstream slaOss, waitOss;
            slaOss << q.targetSlaMinutes << " min";
            waitOss << std::fixed << std::setprecision(1) << q.waitMinutes << " min";
            out << "  " << std::left << std::setw(12) << q.orderId
                << std::setw(22) << q.itemName
                << std::setw(14) << stationTypeToString(q.stationType)
                << std::setw(12) << slaOss.str()
                << std::setw(12) << waitOss.str()
                << std::setw(14) << (q.isBreaching ? "SLA Breached" : "Normal")
                << "\n";
        }
    }

    out << "\nCompleted Items:\n";
    if (statusData.completedItems.empty()) {
        out << "  No items completed yet.\n";
    } else {
        out << "  " << std::left << std::setw(12) << "Order"
            << std::setw(22) << "Item"
            << std::setw(10) << "Station"
            << std::setw(14) << "Completed At"
            << std::setw(12) << "Prep Time"
            << std::setw(12) << "Target SLA"
            << std::setw(14) << "SLA Status"
            << "\n";
        for (const auto& c : statusData.completedItems) {
            std::ostringstream prepOss, slaOss;
            prepOss << std::fixed << std::setprecision(1) << c.actualPrepMinutes << " min";
            slaOss << c.targetSlaMinutes << " min";
            out << "  " << std::left << std::setw(12) << c.orderId
                << std::setw(22) << c.itemName
                << std::setw(10) << c.stationId
                << std::setw(14) << c.completionTimeStr
                << std::setw(12) << prepOss.str()
                << std::setw(12) << slaOss.str()
                << std::setw(14) << slaStatusToString(c.slaStatus)
                << "\n";
        }
    }
}

}
