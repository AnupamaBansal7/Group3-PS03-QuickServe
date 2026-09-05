#ifndef QUICKSERVE_CLI_MAINMENU_H
#define QUICKSERVE_CLI_MAINMENU_H

#include "cli/OrderCLI.h"
#include "cli/KitchenStatusCLI.h"
#include "cli/CompletionCLI.h"
#include "cli/ReportCLI.h"
#include "services/IClock.h"
#include <memory>
#include <iostream>

namespace QuickServe {

class MainMenu {
public:
    MainMenu(std::shared_ptr<OrderCLI> orderCli,
             std::shared_ptr<KitchenStatusCLI> kitchenStatusCli,
             std::shared_ptr<CompletionCLI> completionCli,
             std::shared_ptr<ReportCLI> reportCli,
             std::shared_ptr<IClock> clock);

    void run(std::istream& in = std::cin, std::ostream& out = std::cout);

private:
    void displayMenu(std::ostream& out) const;

    std::shared_ptr<OrderCLI> orderCli_;
    std::shared_ptr<KitchenStatusCLI> kitchenStatusCli_;
    std::shared_ptr<CompletionCLI> completionCli_;
    std::shared_ptr<ReportCLI> reportCli_;
    std::shared_ptr<IClock> clock_;
};

} // namespace QuickServe

#endif // QUICKSERVE_CLI_MAINMENU_H
