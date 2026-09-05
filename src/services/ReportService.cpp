#include "services/ReportService.h"
#include <algorithm>

namespace QuickServe {

ReportService::ReportService(std::shared_ptr<FacilityRepository> facilityRepo,
                             std::shared_ptr<OrderRepository> orderRepo,
                             std::shared_ptr<QueueService> queueService,
                             std::shared_ptr<IClock> clock,
                             double defaultShiftMinutes)
    : facilityRepo_(facilityRepo),
      orderRepo_(orderRepo),
      queueService_(queueService),
      clock_(clock),
      shiftDurationMinutes_(defaultShiftMinutes > 0 ? defaultShiftMinutes : 60.0) {}

KitchenStatusData ReportService::getKitchenStatus() const {
    KitchenStatusData data;
    auto fac = facilityRepo_->getActiveFacility();
    if (!fac) {
        data.facilityName = "No Active Facility";
        return data;
    }
    data.facilityName = fac->getName();

    StationType types[] = {
        StationType::GRILL,
        StationType::FRYER,
        StationType::COLD_PREP,
        StationType::TANDOOR
    };

    for (StationType t : types) {
        StationTypeSummary summary;
        summary.type = t;
        summary.freeCount = fac->getFreeStationCount(t);
        summary.busyCount = fac->getBusyStationCount(t);
        summary.queuedCount = static_cast<int>(queueService_->getQueueSize(t));
        data.typeSummaries.push_back(summary);
    }

    for (const auto& st : fac->getAllStations()) {
        StationStatusItem item;
        item.stationId = st->getStationId();
        item.type = st->getStationType();
        item.status = st->getStatus();
        if (st->isBusy() && st->getCurrentOrderItem()) {
            item.currentOrderId = st->getCurrentOrderItem()->getOrderId();
            item.currentItemName = st->getCurrentOrderItem()->getMenuItem().getName();
        } else {
            item.currentOrderId = "";
            item.currentItemName = "";
        }
        data.stationDetails.push_back(item);
    }

    return data;
}

ShiftReportData ReportService::getShiftReport(double shiftDurationMinutes) const {
    ShiftReportData report;
    auto fac = facilityRepo_->getActiveFacility();
    if (!fac) {
        report.facilityName = "No Active Facility";
        return report;
    }

    double shiftMins = (shiftDurationMinutes > 0.0) ? shiftDurationMinutes : shiftDurationMinutes_;
    report.facilityName = fac->getName();
    report.shiftDurationMinutes = shiftMins;

    auto now = clock_->now();

    StationType types[] = {
        StationType::GRILL,
        StationType::FRYER,
        StationType::COLD_PREP,
        StationType::TANDOOR
    };

    for (StationType t : types) {
        auto stations = fac->getStationsByType(t);
        double totalBusy = 0.0;
        int breaches = 0;

        for (const auto& st : stations) {
            totalBusy += st->getEffectiveBusyMinutes(now, shiftMins);
            breaches += st->getSlaBreachCount();
        }

        double totalCapacity = static_cast<double>(stations.size()) * shiftMins;
        double util = 0.0;
        if (totalCapacity > 0.0) {
            util = (totalBusy / totalCapacity) * 100.0;
            if (util > 100.0) util = 100.0;
        }

        StationTypeReportItem item;
        item.type = t;
        item.utilizationPercent = util;
        item.slaBreaches = breaches;
        item.totalBusyMinutes = totalBusy;
        report.stationTypeReports.push_back(item);
    }

    // Individual station utilization & SLA breach statistics
    for (const auto& st : fac->getAllStations()) {
        StationReportItem stItem;
        stItem.stationId = st->getStationId();
        stItem.type = st->getStationType();
        stItem.busyMinutes = st->getEffectiveBusyMinutes(now, shiftMins);
        stItem.utilizationPercent = st->getUtilization(shiftMins, now);
        stItem.slaBreaches = st->getSlaBreachCount();
        report.individualStationReports.push_back(stItem);
    }

    // Totals across all orders
    auto orders = orderRepo_->getAll();
    report.totalOrders = static_cast<int>(orders.size());
    report.totalItems = 0;
    report.completedItems = 0;
    report.totalSlaBreaches = 0;

    for (const auto& ord : orders) {
        for (const auto& item : ord->getOrderItems()) {
            report.totalItems++;
            if (item->getStatus() == OrderItemStatus::COMPLETED) {
                report.completedItems++;
                if (item->getSlaStatus() == SlaStatus::BREACHED) {
                    report.totalSlaBreaches++;
                }
            }
        }
    }

    report.currentlyQueued = static_cast<int>(queueService_->getTotalQueueSize());

    if (report.completedItems > 0) {
        int withinSla = report.completedItems - report.totalSlaBreaches;
        if (withinSla < 0) withinSla = 0;
        report.slaCompliancePercent = (static_cast<double>(withinSla) / static_cast<double>(report.completedItems)) * 100.0;
    } else {
        report.slaCompliancePercent = 100.0;
    }

    return report;
}

void ReportService::setShiftDurationMinutes(double minutes) {
    if (minutes > 0.0) {
        shiftDurationMinutes_ = minutes;
    }
}

double ReportService::getShiftDurationMinutes() const {
    return shiftDurationMinutes_;
}

} // namespace QuickServe
