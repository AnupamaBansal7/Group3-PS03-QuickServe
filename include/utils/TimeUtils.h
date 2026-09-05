#ifndef QUICKSERVE_UTILS_TIMEUTILS_H
#define QUICKSERVE_UTILS_TIMEUTILS_H

#include <chrono>
#include <string>

namespace QuickServe {

class TimeUtils {
public:
    static std::string formatTimePoint(std::chrono::system_clock::time_point tp);
    static double durationMinutes(std::chrono::system_clock::time_point start,
                                  std::chrono::system_clock::time_point end);
};

} // namespace QuickServe

#endif // QUICKSERVE_UTILS_TIMEUTILS_H
