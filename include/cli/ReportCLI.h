#ifndef QUICKSERVE_CLI_REPORTCLI_H
#define QUICKSERVE_CLI_REPORTCLI_H

#include "services/ReportService.h"
#include <memory>
#include <iostream>

namespace QuickServe {

class ReportCLI {
public:
    explicit ReportCLI(std::shared_ptr<ReportService> reportService);

    void display(std::ostream& out = std::cout);

private:
    std::shared_ptr<ReportService> reportService_;
};

} // namespace QuickServe

#endif // QUICKSERVE_CLI_REPORTCLI_H
