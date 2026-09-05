#ifndef QUICKSERVE_UTILS_IDGENERATOR_H
#define QUICKSERVE_UTILS_IDGENERATOR_H

#include <string>
#include <atomic>

namespace QuickServe {

class IdGenerator {
public:
    explicit IdGenerator(int startId = 1001);

    std::string nextOrderId();
    std::string nextOrderItemId(const std::string& orderId, int seq);

    void reset(int startId = 1001);

private:
    std::atomic<int> currentId_;
};

} // namespace QuickServe

#endif // QUICKSERVE_UTILS_IDGENERATOR_H
