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
            if (st.plannedMinutes > 0.0) {
                out << " [Prep: " << std::fixed << std::setprecision(1) << st.elapsedMinutes
                    << "/" << st.plannedMinutes << " min]";
            }
        }
        out << "\n";
    }

    // Queued Items Section
    out << "\n========================================\n";
    out << "QUEUED ITEMS (Waiting for Station)\n";
    out << "==================================\n";
    if (statusData.queuedItems.empty()) {
        out << "No items currently in queue.\n";
    } else {
        out << std::left << std::setw(12) << "Order ID"
            << std::setw(20) << "Item Name"
            << std::setw(14) << "Station Type"
            << std::setw(12) << "Target SLA"
            << std::setw(12) << "Wait Time"
            << std::setw(12) << "Priority"
            << "\n";
        out << "--------------------------------------------------------------------------------\n";
        for (const auto& q : statusData.queuedItems) {
            std::ostringstream slaOss, waitOss;
            slaOss << q.targetSlaMinutes << " min";
            waitOss << std::fixed << std::setprecision(1) << q.waitMinutes << " min";
            out << std::left << std::setw(12) << q.orderId
                << std::setw(20) << q.itemName
                << std::setw(14) << stationTypeToString(q.stationType)
                << std::setw(12) << slaOss.str()
                << std::setw(12) << waitOss.str()
                << std::setw(12) << (q.isBreaching ? "SLA BREACH (HIGH)" : "NORMAL")
                << "\n";
        }
    }

    // Completed Items Section
    out << "\n========================================\n";
    out << "COMPLETED ORDERS & ITEMS\n";
    out << "========================\n";
    if (statusData.completedItems.empty()) {
        out << "No items completed yet.\n";
    } else {
        out << std::left << std::setw(12) << "Order ID"
            << std::setw(20) << "Item Name"
            << std::setw(10) << "Station"
            << std::setw(14) << "Completed At"
            << std::setw(12) << "Prep Time"
            << std::setw(12) << "Target SLA"
            << std::setw(12) << "SLA Status"
            << "\n";
        out << "--------------------------------------------------------------------------------------------\n";
        for (const auto& c : statusData.completedItems) {
            std::ostringstream prepOss, slaOss;
            prepOss << std::fixed << std::setprecision(1) << c.actualPrepMinutes << " min";
            slaOss << c.targetSlaMinutes << " min";
            out << std::left << std::setw(12) << c.orderId
                << std::setw(20) << c.itemName
                << std::setw(10) << c.stationId
                << std::setw(14) << c.completionTimeStr
                << std::setw(12) << prepOss.str()
                << std::setw(12) << slaOss.str()
                << std::setw(12) << slaStatusToString(c.slaStatus)
                << "\n";
        }
    }
}

} // namespace QuickServe
