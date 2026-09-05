#ifndef QUICKSERVE_CLI_COMPLETIONCLI_H
#define QUICKSERVE_CLI_COMPLETIONCLI_H

#include "services/StationService.h"
#include <memory>
#include <iostream>

namespace QuickServe {

class CompletionCLI {
public:
    explicit CompletionCLI(std::shared_ptr<StationService> stationService);

    void display(std::istream& in = std::cin, std::ostream& out = std::cout);

private:
    std::shared_ptr<StationService> stationService_;
};

} // namespace QuickServe

#endif // QUICKSERVE_CLI_COMPLETIONCLI_H
