#ifndef QUICKSERVE_MODELS_ORDERITEM_H
#define QUICKSERVE_MODELS_ORDERITEM_H

#include "models/Enums.h"
#include "models/MenuItem.h"
#include <string>
#include <chrono>

namespace QuickServe {

class OrderItem {
public:
    OrderItem(const std::string& orderItemId,
              const std::string& orderId,
              const MenuItem& menuItem,
              std::chrono::system_clock::time_point arrivalTime);

    const std::string& getOrderItemId() const;
    const std::string& getOrderId() const;
    const MenuItem& getMenuItem() const;

    std::chrono::system_clock::time_point getArrivalTime() const;
    std::chrono::system_clock::time_point getStartTime() const;
    void setStartTime(std::chrono::system_clock::time_point t);

    std::chrono::system_clock::time_point getCompletionTime() const;
    void setCompletionTime(std::chrono::system_clock::time_point t);

    const std::string& getAssignedStationId() const;
    void setAssignedStationId(const std::string& stationId);

    double getActualPrepMinutes() const;
    void setActualPrepMinutes(double minutes);

    int getSlaMinutes() const;

    OrderItemStatus getStatus() const;
    void setStatus(OrderItemStatus status);

    SlaStatus getSlaStatus() const;
    void setSlaStatus(SlaStatus status);

    double getPlannedPrepMinutes() const;
    void setPlannedPrepMinutes(double minutes);

    bool isSlaBreached() const { return slaStatus_ == SlaStatus::BREACHED; }
    bool hasBreachedSla() const { return slaStatus_ == SlaStatus::BREACHED; }

private:
    std::string orderItemId_;
    std::string orderId_;
    MenuItem menuItem_;
    std::chrono::system_clock::time_point arrivalTime_;
    std::chrono::system_clock::time_point startTime_;
    std::chrono::system_clock::time_point completionTime_;
    std::string assignedStationId_;
    double plannedPrepMinutes_;
    double actualPrepMinutes_;
    int slaMinutes_;
    OrderItemStatus status_;
    SlaStatus slaStatus_;
};

} // namespace QuickServe

#endif // QUICKSERVE_MODELS_ORDERITEM_H
