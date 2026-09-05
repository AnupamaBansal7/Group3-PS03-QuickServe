#include "utils/TimeUtils.h"
#include <iomanip>
#include <sstream>
#include <ctime>

namespace QuickServe {

std::string TimeUtils::formatTimePoint(std::chrono::system_clock::time_point tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm* tmPtr = std::localtime(&t);
    if (!tmPtr) return "00:00:00";

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << tmPtr->tm_hour << ":"
        << std::setw(2) << tmPtr->tm_min << ":"
        << std::setw(2) << tmPtr->tm_sec;
    return oss.str();
}

double TimeUtils::durationMinutes(std::chrono::system_clock::time_point start,
                                  std::chrono::system_clock::time_point end) {
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    return diff.count() / 60000.0;
}

} // namespace QuickServe
