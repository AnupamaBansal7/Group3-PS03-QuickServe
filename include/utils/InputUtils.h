#ifndef QUICKSERVE_UTILS_INPUTUTILS_H
#define QUICKSERVE_UTILS_INPUTUTILS_H

#include <string>
#include <vector>
#include <iostream>

namespace QuickServe {

class InputUtils {
public:
    static std::string readLine(std::istream& in = std::cin);
    static std::string readTrimmedString(const std::string& prompt, std::istream& in = std::cin, std::ostream& out = std::cout);
    static int readInt(const std::string& prompt, int minVal, int maxVal, std::istream& in = std::cin, std::ostream& out = std::cout);
    static std::vector<int> readIntList(const std::string& prompt, std::istream& in = std::cin, std::ostream& out = std::cout);
};

} // namespace QuickServe

#endif // QUICKSERVE_UTILS_INPUTUTILS_H
