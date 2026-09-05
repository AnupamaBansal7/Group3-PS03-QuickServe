#ifndef QUICKSERVE_CLI_KITCHENSTATUSCLI_H
#define QUICKSERVE_CLI_KITCHENSTATUSCLI_H

#include "services/ReportService.h"
#include <memory>
#include <iostream>

namespace QuickServe {

class KitchenStatusCLI {
public:
    explicit KitchenStatusCLI(std::shared_ptr<ReportService> reportService);

    void display(std::ostream& out = std::cout);

private:
    std::shared_ptr<ReportService> reportService_;
};

} // namespace QuickServe

#endif // QUICKSERVE_CLI_KITCHENSTATUSCLI_H
