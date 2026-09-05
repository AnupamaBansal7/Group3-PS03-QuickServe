#ifndef QUICKSERVE_SERVICES_STATIONSERVICE_H
#define QUICKSERVE_SERVICES_STATIONSERVICE_H

#include "models/Station.h"
#include "models/OrderItem.h"
#include "repositories/FacilityRepository.h"
#include "services/IClock.h"
#include "services/SlaService.h"
#include "services/QueueService.h"
#include <string>
#include <vector>
#include <memory>

namespace QuickServe {

struct CompletionResult {
    bool success;
    std::string errorMessage;
    std::shared_ptr<Station> station;
    std::shared_ptr<OrderItem> completedItem;
    double actualPrepMinutes;
    int targetSlaMinutes;
    SlaStatus slaStatus;
    std::shared_ptr<OrderItem> nextAssignedItem;

    bool isBreached() const { return slaStatus == SlaStatus::BREACHED; }
};

class StationService {
public:
    StationService(std::shared_ptr<FacilityRepository> facilityRepo,
                   std::shared_ptr<IClock> clock,
                   std::shared_ptr<SlaService> slaService,
                   std::shared_ptr<QueueService> queueService);

    std::shared_ptr<Station> findStation(const std::string& stationId) const;
    std::shared_ptr<Station> findFreeStation(StationType type) const;

    std::vector<std::shared_ptr<Station>> getAllStations() const;
    std::vector<std::shared_ptr<Station>> getActiveStations() const;

    bool assignItem(std::shared_ptr<Station> station, std::shared_ptr<OrderItem> item);

    CompletionResult completeStationItem(const std::string& stationId, double simulatedPrepMinutes = -1.0);

private:
    std::shared_ptr<FacilityRepository> facilityRepo_;
    std::shared_ptr<IClock> clock_;
    std::shared_ptr<SlaService> slaService_;
    std::shared_ptr<QueueService> queueService_;
};

} // namespace QuickServe

#endif // QUICKSERVE_SERVICES_STATIONSERVICE_H
