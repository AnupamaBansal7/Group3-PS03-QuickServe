#include "models/Enums.h"
#include <algorithm>
#include <cctype>

namespace QuickServe {

std::string stationTypeToString(StationType type) {
    switch (type) {
        case StationType::GRILL:     return "GRILL";
        case StationType::FRYER:     return "FRYER";
        case StationType::COLD_PREP: return "COLD_PREP";
        case StationType::TANDOOR:   return "TANDOOR";
    }
    return "UNKNOWN";
}

StationType stringToStationType(const std::string& str) {
    std::string upper = str;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    if (upper == "GRILL") return StationType::GRILL;
    if (upper == "FRYER") return StationType::FRYER;
    if (upper == "COLD_PREP" || upper == "COLD-PREP" || upper == "COLDPREP") return StationType::COLD_PREP;
    if (upper == "TANDOOR") return StationType::TANDOOR;
    return StationType::GRILL;
}

std::string stationStatusToString(StationStatus status) {
    switch (status) {
        case StationStatus::FREE: return "Free";
        case StationStatus::BUSY: return "Busy";
    }
    return "Unknown";
}

std::string orderItemStatusToString(OrderItemStatus status) {
    switch (status) {
        case OrderItemStatus::QUEUED:    return "Queued";
        case OrderItemStatus::PREPARING: return "Preparing";
        case OrderItemStatus::COMPLETED: return "Completed";
    }
    return "Unknown";
}

std::string slaStatusToString(SlaStatus status) {
    switch (status) {
        case SlaStatus::PENDING:    return "Pending";
        case SlaStatus::WITHIN_SLA: return "Within SLA";
        case SlaStatus::BREACHED:   return "Breached";
    }
    return "Unknown";
}

}
