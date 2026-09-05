#include "utils/InputUtils.h"
#include <sstream>
#include <algorithm>

namespace QuickServe {

std::string InputUtils::readLine(std::istream& in) {
    std::string line;
    if (std::getline(in, line)) {
        return line;
    }
    return "";
}

std::string InputUtils::readTrimmedString(const std::string& prompt, std::istream& in, std::ostream& out) {
    while (true) {
        out << prompt;
        std::string line = readLine(in);
        if (in.eof()) {
            return "";
        }
        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            out << "Input cannot be empty. Please try again.\n";
            continue;
        }
        size_t last = line.find_last_not_of(" \t\r\n");
        return line.substr(first, (last - first + 1));
    }
}

int InputUtils::readInt(const std::string& prompt, int minVal, int maxVal, std::istream& in, std::ostream& out) {
    while (true) {
        out << prompt;
        std::string line = readLine(in);
        if (in.eof()) {
            return minVal;
        }
        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            out << "Input cannot be empty. Please enter a number.\n";
            continue;
        }
        size_t last = line.find_last_not_of(" \t\r\n");
        std::string trimmed = line.substr(first, (last - first + 1));

        try {
            size_t idx = 0;
            int val = std::stoi(trimmed, &idx);
            if (idx != trimmed.length()) {
                out << "Invalid input '" << trimmed << "'. Please enter a valid number.\n";
                continue;
            }
            if (val < minVal || val > maxVal) {
                out << "Please enter a number between " << minVal << " and " << maxVal << ".\n";
                continue;
            }
            return val;
        } catch (...) {
            out << "Invalid number format. Please enter a valid integer.\n";
        }
    }
}

std::vector<int> InputUtils::readIntList(const std::string& prompt, std::istream& in, std::ostream& out) {
    while (true) {
        out << prompt;
        std::string line = readLine(in);
        if (in.eof()) {
            return {};
        }
        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            out << "Input cannot be empty. Please enter item IDs separated by spaces.\n";
            continue;
        }

        std::istringstream iss(line);
        std::vector<int> list;
        std::string token;
        bool valid = true;

        while (iss >> token) {
            try {
                size_t idx = 0;
                int val = std::stoi(token, &idx);
                if (idx != token.length()) {
                    out << "Invalid item ID '" << token << "'. Please enter numeric IDs.\n";
                    valid = false;
                    break;
                }
                list.push_back(val);
            } catch (...) {
                out << "Invalid item ID '" << token << "'. Please enter numeric IDs.\n";
                valid = false;
                break;
            }
        }

        if (valid) {
            if (list.empty()) {
                out << "Please enter at least one item ID.\n";
                continue;
            }
            return list;
        }
    }
}

} // namespace QuickServe
