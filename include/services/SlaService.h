#ifndef QUICKSERVE_SERVICES_SLASERVICE_H
#define QUICKSERVE_SERVICES_SLASERVICE_H

#include "models/OrderItem.h"
#include "models/Enums.h"
#include "services/IClock.h"
#include <memory>
#include <chrono>

namespace QuickServe {

class SlaService {
public:
    explicit SlaService(std::shared_ptr<IClock> clock);

    double calculateActualPrepMinutes(std::chrono::system_clock::time_point start,
                                     std::chrono::system_clock::time_point end) const;

    SlaStatus evaluatePrepSla(double actualPrepMinutes, int targetSlaMinutes) const;

    bool isQueuedItemBreached(const OrderItem& item,
                             std::chrono::system_clock::time_point currentTime) const;

    double getQueuedWaitMinutes(const OrderItem& item,
                               std::chrono::system_clock::time_point currentTime) const;

private:
    std::shared_ptr<IClock> clock_;
};

} // namespace QuickServe

#endif // QUICKSERVE_SERVICES_SLASERVICE_H
