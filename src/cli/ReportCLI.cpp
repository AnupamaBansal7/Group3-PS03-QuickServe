#include "cli/ReportCLI.h"
#include <iomanip>

namespace QuickServe {

ReportCLI::ReportCLI(std::shared_ptr<ReportService> reportService)
    : reportService_(reportService) {}

void ReportCLI::display(std::ostream& out) {
    auto report = reportService_->getShiftReport();

    out << "\nShift Report (" << report.facilityName << ")\n";
    out << "Shift Duration: ";
    if (report.shiftDurationMinutes == static_cast<long long>(report.shiftDurationMinutes)) {
        out << static_cast<long long>(report.shiftDurationMinutes);
    } else {
        out << std::fixed << std::setprecision(1) << report.shiftDurationMinutes;
    }
    out << " minutes\n\n";

    out << "Station Types:\n";
    out << "  " << std::left << std::setw(16) << "Station Type"
        << std::setw(16) << "Utilization"
        << std::setw(14) << "SLA Breaches"
        << "\n";

    for (const auto& sr : report.stationTypeReports) {
        std::ostringstream utilOss;
        utilOss << std::fixed << std::setprecision(1) << sr.utilizationPercent << "%";
        out << "  " << std::left << std::setw(16) << stationTypeToString(sr.type)
            << std::setw(16) << utilOss.str()
            << std::setw(14) << sr.slaBreaches
            << "\n";
    }

    out << "\nIndividual Stations:\n";
    out << "  " << std::left << std::setw(12) << "Station ID"
        << std::setw(14) << "Type"
        << std::setw(16) << "Utilization"
        << std::setw(14) << "SLA Breaches"
        << "\n";

    for (const auto& ir : report.individualStationReports) {
        std::ostringstream utilOss;
        utilOss << std::fixed << std::setprecision(1) << ir.utilizationPercent << "%";
        out << "  " << std::left << std::setw(12) << ir.stationId
            << std::setw(14) << stationTypeToString(ir.type)
            << std::setw(16) << utilOss.str()
            << std::setw(14) << ir.slaBreaches
            << "\n";
    }

    out << "\nShift Summary:\n";
    out << "  Total Orders:      " << report.totalOrders << "\n";
    out << "  Total Items:       " << report.totalItems << "\n";
    out << "  Completed Items:   " << report.completedItems << "\n";
    out << "  Currently Queued:  " << report.currentlyQueued << "\n";
    out << "  SLA Breaches:      " << report.totalSlaBreaches << "\n";

    std::ostringstream compOss;
    compOss << std::fixed << std::setprecision(1) << report.slaCompliancePercent << "%";
    out << "  SLA Compliance:    " << compOss.str() << "\n";
}

}
