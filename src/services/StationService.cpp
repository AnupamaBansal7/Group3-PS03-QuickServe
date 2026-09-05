#include "services/StationService.h"
#include "utils/TimeUtils.h"

namespace QuickServe {

StationService::StationService(std::shared_ptr<FacilityRepository> facilityRepo,
                               std::shared_ptr<IClock> clock,
                               std::shared_ptr<SlaService> slaService,
                               std::shared_ptr<QueueService> queueService)
    : facilityRepo_(facilityRepo),
      clock_(clock),
      slaService_(slaService),
      queueService_(queueService) {}

std::shared_ptr<Station> StationService::findStation(const std::string& stationId) const {
    auto fac = facilityRepo_->getActiveFacility();
    if (!fac) return nullptr;
    return fac->findStation(stationId);
}

std::shared_ptr<Station> StationService::findFreeStation(StationType type) const {
    auto fac = facilityRepo_->getActiveFacility();
    if (!fac) return nullptr;
    return fac->findFreeStation(type);
}

std::vector<std::shared_ptr<Station>> StationService::getAllStations() const {
    auto fac = facilityRepo_->getActiveFacility();
    if (!fac) return {};
    return fac->getAllStations();
}

std::vector<std::shared_ptr<Station>> StationService::getActiveStations() const {
    std::vector<std::shared_ptr<Station>> list;
    auto fac = facilityRepo_->getActiveFacility();
    if (!fac) return list;

    for (const auto& st : fac->getAllStations()) {
        if (st->isBusy() && st->getCurrentOrderItem()) {
            list.push_back(st);
        }
    }
    return list;
}

bool StationService::assignItem(std::shared_ptr<Station> station, std::shared_ptr<OrderItem> item) {
    if (!station || !station->isFree() || !item) {
        return false;
    }
    station->assignOrderItem(item, clock_->now());
    return true;
}

CompletionResult StationService::completeStationItem(const std::string& stationId, double simulatedPrepMinutes) {
    CompletionResult result;
    result.success = false;
    result.station = nullptr;
    result.completedItem = nullptr;
    result.actualPrepMinutes = 0.0;
    result.targetSlaMinutes = 0;
    result.slaStatus = SlaStatus::PENDING;
    result.nextAssignedItem = nullptr;

    auto station = findStation(stationId);
    if (!station) {
        result.errorMessage = "ERROR: Station " + stationId + " does not exist.";
        return result;
    }

    if (station->isFree() || !station->getCurrentOrderItem()) {
        result.errorMessage = "ERROR: Station " + stationId + " is currently free.";
        return result;
    }

    auto item = station->getCurrentOrderItem();
    auto completionTime = (simulatedPrepMinutes >= 0.0)
        ? (item->getStartTime() + std::chrono::milliseconds(static_cast<long long>(simulatedPrepMinutes * 60000.0)))
        : clock_->now();

    // 1. Record completion time & set status to COMPLETED
    item->setCompletionTime(completionTime);
    item->setStatus(OrderItemStatus::COMPLETED);

    // 2. Record actual prep time
    double actualPrep = (simulatedPrepMinutes >= 0.0)
        ? simulatedPrepMinutes
        : slaService_->calculateActualPrepMinutes(item->getStartTime(), completionTime);
    item->setActualPrepMinutes(actualPrep);

    // 3. Flag whether the item breached its SLA
    SlaStatus sla = slaService_->evaluatePrepSla(actualPrep, item->getSlaMinutes());
    item->setSlaStatus(sla);

    // 4. Update station busy minutes & SLA breach counter
    station->addBusyMinutes(actualPrep);
    if (sla == SlaStatus::BREACHED) {
        station->incrementSlaBreachCount();
    }

    // 5. Free the station
    station->releaseOrderItem();

    // 6. Check queue for that station type and automatically assign if an item is waiting
    auto nextQueued = queueService_->getNextItem(station->getStationType());
    if (nextQueued) {
        station->assignOrderItem(nextQueued, completionTime);
        result.nextAssignedItem = nextQueued;
    }

    result.success = true;
    result.station = station;
    result.completedItem = item;
    result.actualPrepMinutes = actualPrep;
    result.targetSlaMinutes = item->getSlaMinutes();
    result.slaStatus = sla;

    return result;
}

std::vector<CompletionResult> StationService::updateCookingProgress(std::chrono::system_clock::time_point currentTime) {
    std::vector<CompletionResult> completed;
    auto fac = facilityRepo_->getActiveFacility();
    if (!fac) return completed;

    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& st : fac->getAllStations()) {
            if (st->isBusy() && st->getCurrentOrderItem()) {
                auto item = st->getCurrentOrderItem();
                double elapsed = TimeUtils::durationMinutes(st->getBusyStartTime(), currentTime);
                double needed = item->getPlannedPrepMinutes();

                if (elapsed >= needed) {
                    auto res = completeStationItem(st->getStationId(), needed);
                    if (res.success) {
                        completed.push_back(res);
                        changed = true;
                        break; // Queue may have assigned a new item to this station; restart loop
                    }
                }
            }
        }
    }
    return completed;
}

} // namespace QuickServe
