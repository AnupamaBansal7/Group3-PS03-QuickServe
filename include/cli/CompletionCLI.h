#ifndef QUICKSERVE_CLI_COMPLETIONCLI_H
#define QUICKSERVE_CLI_COMPLETIONCLI_H

#include "services/StationService.h"
#include "services/IClock.h"
#include <memory>
#include <iostream>

namespace QuickServe {

class CompletionCLI {
public:
    CompletionCLI(std::shared_ptr<StationService> stationService,
                  std::shared_ptr<IClock> clock = nullptr);

    void display(std::istream& in = std::cin, std::ostream& out = std::cout);

private:
    std::shared_ptr<StationService> stationService_;
    std::shared_ptr<IClock> clock_;
};

} // namespace QuickServe

#endif // QUICKSERVE_CLI_COMPLETIONCLI_H
