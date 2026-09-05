#ifndef QUICKSERVE_MODELS_STATION_H
#define QUICKSERVE_MODELS_STATION_H

#include "models/Enums.h"
#include "models/OrderItem.h"
#include <string>
#include <memory>
#include <chrono>

namespace QuickServe {

class Station {
public:
    Station(const std::string& stationId, StationType stationType);

    const std::string& getStationId() const;
    StationType getStationType() const;
    StationStatus getStatus() const;

    bool isFree() const;
    bool isBusy() const;

    std::shared_ptr<OrderItem> getCurrentOrderItem() const;
    std::chrono::system_clock::time_point getBusyStartTime() const;

    void assignOrderItem(std::shared_ptr<OrderItem> item, std::chrono::system_clock::time_point startTime);
    std::shared_ptr<OrderItem> releaseOrderItem();

    double getAccumulatedBusyMinutes() const;
    void addBusyMinutes(double minutes);

    int getSlaBreachCount() const;
    void incrementSlaBreachCount();

    double getEffectiveBusyMinutes(std::chrono::system_clock::time_point currentTime,
                                   double shiftDurationMinutes) const;

    double getUtilization(double shiftDurationMinutes,
                          std::chrono::system_clock::time_point currentTime) const;

private:
    std::string stationId_;
    StationType stationType_;
    StationStatus status_;
    std::shared_ptr<OrderItem> currentOrderItem_;
    std::chrono::system_clock::time_point busyStartTime_;
    double accumulatedBusyMinutes_;
    int slaBreachCount_;
};

} // namespace QuickServe

#endif // QUICKSERVE_MODELS_STATION_H
