#include "models/Station.h"
#include "utils/TimeUtils.h"
#include <algorithm>

namespace QuickServe {

Station::Station(const std::string& stationId, StationType stationType)
    : stationId_(stationId),
      stationType_(stationType),
      status_(StationStatus::FREE),
      currentOrderItem_(nullptr),
      busyStartTime_(),
      accumulatedBusyMinutes_(0.0),
      slaBreachCount_(0) {}

const std::string& Station::getStationId() const {
    return stationId_;
}

StationType Station::getStationType() const {
    return stationType_;
}

StationStatus Station::getStatus() const {
    return status_;
}

bool Station::isFree() const {
    return status_ == StationStatus::FREE;
}

bool Station::isBusy() const {
    return status_ == StationStatus::BUSY;
}

std::shared_ptr<OrderItem> Station::getCurrentOrderItem() const {
    return currentOrderItem_;
}

std::chrono::system_clock::time_point Station::getBusyStartTime() const {
    return busyStartTime_;
}

void Station::assignOrderItem(std::shared_ptr<OrderItem> item, std::chrono::system_clock::time_point startTime) {
    currentOrderItem_ = item;
    busyStartTime_ = startTime;
    status_ = StationStatus::BUSY;

    if (item) {
        item->setAssignedStationId(stationId_);
        item->setStartTime(startTime);
        item->setStatus(OrderItemStatus::PREPARING);
    }
}

std::shared_ptr<OrderItem> Station::releaseOrderItem() {
    auto previous = currentOrderItem_;
    currentOrderItem_ = nullptr;
    status_ = StationStatus::FREE;
    return previous;
}

double Station::getAccumulatedBusyMinutes() const {
    return accumulatedBusyMinutes_;
}

void Station::addBusyMinutes(double minutes) {
    if (minutes > 0.0) {
        accumulatedBusyMinutes_ += minutes;
    }
}

int Station::getSlaBreachCount() const {
    return slaBreachCount_;
}

void Station::incrementSlaBreachCount() {
    slaBreachCount_++;
}

double Station::getEffectiveBusyMinutes(std::chrono::system_clock::time_point currentTime,
                                       double shiftDurationMinutes) const {
    double busy = accumulatedBusyMinutes_;
    if (status_ == StationStatus::BUSY) {
        double ongoing = TimeUtils::durationMinutes(busyStartTime_, currentTime);
        if (ongoing > 0.0) {
            busy += ongoing;
        }
    }
    if (shiftDurationMinutes > 0.0 && busy > shiftDurationMinutes) {
        busy = shiftDurationMinutes;
    }
    return busy;
}

double Station::getUtilization(double shiftDurationMinutes,
                              std::chrono::system_clock::time_point currentTime) const {
    if (shiftDurationMinutes <= 0.0) {
        return 0.0;
    }
    double eff = getEffectiveBusyMinutes(currentTime, shiftDurationMinutes);
    double util = (eff / shiftDurationMinutes) * 100.0;
    return (util > 100.0) ? 100.0 : (util < 0.0 ? 0.0 : util);
}

} // namespace QuickServe
