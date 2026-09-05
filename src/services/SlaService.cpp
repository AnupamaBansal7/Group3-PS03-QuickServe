#include "services/SlaService.h"
#include "utils/TimeUtils.h"

namespace QuickServe {

SlaService::SlaService(std::shared_ptr<IClock> clock) : clock_(clock) {}

double SlaService::calculateActualPrepMinutes(std::chrono::system_clock::time_point start,
                                             std::chrono::system_clock::time_point end) const {
    double mins = TimeUtils::durationMinutes(start, end);
    return mins > 0.0 ? mins : 0.0;
}

SlaStatus SlaService::evaluatePrepSla(double actualPrepMinutes, int targetSlaMinutes) const {
    if (actualPrepMinutes > static_cast<double>(targetSlaMinutes)) {
        return SlaStatus::BREACHED;
    }
    return SlaStatus::WITHIN_SLA;
}

bool SlaService::isQueuedItemBreached(const OrderItem& item,
                                     std::chrono::system_clock::time_point currentTime) const {
    double waitMins = getQueuedWaitMinutes(item, currentTime);
    return waitMins > static_cast<double>(item.getSlaMinutes());
}

double SlaService::getQueuedWaitMinutes(const OrderItem& item,
                                       std::chrono::system_clock::time_point currentTime) const {
    return TimeUtils::durationMinutes(item.getArrivalTime(), currentTime);
}

} // namespace QuickServe
