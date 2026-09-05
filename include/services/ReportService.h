#ifndef QUICKSERVE_SERVICES_REPORTSERVICE_H
#define QUICKSERVE_SERVICES_REPORTSERVICE_H

#include "models/Enums.h"
#include "repositories/FacilityRepository.h"
#include "repositories/OrderRepository.h"
#include "services/QueueService.h"
#include "services/IClock.h"
#include <string>
#include <vector>
#include <memory>

namespace QuickServe {

struct StationTypeSummary {
    StationType type;
    int freeCount;
    int busyCount;
    int queuedCount;
};

struct StationStatusItem {
    std::string stationId;
    StationType type;
    StationStatus status;
    std::string currentOrderId;
    std::string currentItemName;
};

struct KitchenStatusData {
    std::string facilityName;
    std::vector<StationTypeSummary> typeSummaries;
    std::vector<StationStatusItem> stationDetails;
};

struct StationTypeReportItem {
    StationType type;
    double utilizationPercent;
    int slaBreaches;
    double totalBusyMinutes;
};

struct StationReportItem {
    std::string stationId;
    StationType type;
    double utilizationPercent;
    int slaBreaches;
    double busyMinutes;
};

struct ShiftReportData {
    std::string facilityName;
    double shiftDurationMinutes;
    std::vector<StationTypeReportItem> stationTypeReports;
    std::vector<StationReportItem> individualStationReports;
    int totalOrders;
    int totalItems;
    int completedItems;
    int currentlyQueued;
    int totalSlaBreaches;
    double slaCompliancePercent;
};

class ReportService {
public:
    ReportService(std::shared_ptr<FacilityRepository> facilityRepo,
                  std::shared_ptr<OrderRepository> orderRepo,
                  std::shared_ptr<QueueService> queueService,
                  std::shared_ptr<IClock> clock,
                  double defaultShiftMinutes = 60.0);

    KitchenStatusData getKitchenStatus() const;
    ShiftReportData getShiftReport(double shiftDurationMinutes = -1.0) const;

    void setShiftDurationMinutes(double minutes);
    double getShiftDurationMinutes() const;

private:
    std::shared_ptr<FacilityRepository> facilityRepo_;
    std::shared_ptr<OrderRepository> orderRepo_;
    std::shared_ptr<QueueService> queueService_;
    std::shared_ptr<IClock> clock_;
    double shiftDurationMinutes_;
};

} // namespace QuickServe

#endif // QUICKSERVE_SERVICES_REPORTSERVICE_H
