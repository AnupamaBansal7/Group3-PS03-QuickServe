#include "models/OrderItem.h"

namespace QuickServe {

OrderItem::OrderItem(const std::string& orderItemId,
                     const std::string& orderId,
                     const MenuItem& menuItem,
                     std::chrono::system_clock::time_point arrivalTime)
    : orderItemId_(orderItemId),
      orderId_(orderId),
      menuItem_(menuItem),
      arrivalTime_(arrivalTime),
      startTime_(),
      completionTime_(),
      assignedStationId_(""),
      plannedPrepMinutes_(0.0),
      actualPrepMinutes_(0.0),
      slaMinutes_(menuItem.getSlaMinutes()),
      status_(OrderItemStatus::QUEUED),
      slaStatus_(SlaStatus::PENDING) {
    // Realistic kitchen prep variance: ~30% of items experience slight kitchen delay & breach SLA
    static const double varianceMultipliers[] = {0.88, 1.15, 0.92, 1.20, 0.85, 1.12, 0.96, 1.24};
    static size_t varianceIdx = 0;
    double factor = varianceMultipliers[varianceIdx % 8];
    varianceIdx++;
    plannedPrepMinutes_ = static_cast<double>(menuItem.getSlaMinutes()) * factor;
}

const std::string& OrderItem::getOrderItemId() const {
    return orderItemId_;
}

const std::string& OrderItem::getOrderId() const {
    return orderId_;
}

const MenuItem& OrderItem::getMenuItem() const {
    return menuItem_;
}

std::chrono::system_clock::time_point OrderItem::getArrivalTime() const {
    return arrivalTime_;
}

std::chrono::system_clock::time_point OrderItem::getStartTime() const {
    return startTime_;
}

void OrderItem::setStartTime(std::chrono::system_clock::time_point t) {
    startTime_ = t;
}

std::chrono::system_clock::time_point OrderItem::getCompletionTime() const {
    return completionTime_;
}

void OrderItem::setCompletionTime(std::chrono::system_clock::time_point t) {
    completionTime_ = t;
}

const std::string& OrderItem::getAssignedStationId() const {
    return assignedStationId_;
}

void OrderItem::setAssignedStationId(const std::string& stationId) {
    assignedStationId_ = stationId;
}

double OrderItem::getActualPrepMinutes() const {
    return actualPrepMinutes_;
}

void OrderItem::setActualPrepMinutes(double minutes) {
    actualPrepMinutes_ = minutes;
}

double OrderItem::getPlannedPrepMinutes() const {
    return plannedPrepMinutes_;
}

void OrderItem::setPlannedPrepMinutes(double minutes) {
    plannedPrepMinutes_ = minutes;
}

int OrderItem::getSlaMinutes() const {
    return slaMinutes_;
}

OrderItemStatus OrderItem::getStatus() const {
    return status_;
}

void OrderItem::setStatus(OrderItemStatus status) {
    status_ = status;
}

SlaStatus OrderItem::getSlaStatus() const {
    return slaStatus_;
}

void OrderItem::setSlaStatus(SlaStatus status) {
    slaStatus_ = status;
}

} // namespace QuickServe
