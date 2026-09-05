#ifndef QUICKSERVE_MODELS_ENUMS_H
#define QUICKSERVE_MODELS_ENUMS_H

#include <string>

namespace QuickServe {

enum class StationType {
    GRILL,
    FRYER,
    COLD_PREP,
    TANDOOR
};

enum class StationStatus {
    FREE,
    BUSY
};

enum class OrderItemStatus {
    QUEUED,
    PREPARING,
    COMPLETED
};

enum class SlaStatus {
    PENDING,
    WITHIN_SLA,
    BREACHED
};

std::string stationTypeToString(StationType type);
StationType stringToStationType(const std::string& str);

std::string stationStatusToString(StationStatus status);
std::string orderItemStatusToString(OrderItemStatus status);
std::string slaStatusToString(SlaStatus status);

} // namespace QuickServe

#endif // QUICKSERVE_MODELS_ENUMS_H
