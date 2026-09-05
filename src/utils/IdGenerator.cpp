#include "utils/IdGenerator.h"
#include <sstream>

namespace QuickServe {

IdGenerator::IdGenerator(int startId) : currentId_(startId) {}

std::string IdGenerator::nextOrderId() {
    int id = currentId_++;
    std::ostringstream oss;
    oss << "ORD-" << id;
    return oss.str();
}

std::string IdGenerator::nextOrderItemId(const std::string& orderId, int seq) {
    std::ostringstream oss;
    oss << orderId << "-ITEM-" << seq;
    return oss.str();
}

void IdGenerator::reset(int startId) {
    currentId_ = startId;
}

} // namespace QuickServe
