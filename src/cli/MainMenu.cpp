#include "cli/MainMenu.h"
#include "utils/InputUtils.h"

namespace QuickServe {

MainMenu::MainMenu(std::shared_ptr<OrderCLI> orderCli,
                   std::shared_ptr<KitchenStatusCLI> kitchenStatusCli,
                   std::shared_ptr<CompletionCLI> completionCli,
                   std::shared_ptr<ReportCLI> reportCli,
                   std::shared_ptr<IClock> clock)
    : orderCli_(orderCli),
      kitchenStatusCli_(kitchenStatusCli),
      completionCli_(completionCli),
      reportCli_(reportCli),
      clock_(clock) {}

void MainMenu::displayMenu(std::ostream& out) const {
    out << "\n========================================\n";
    out << "QUICKSERVE KITCHEN SYSTEM\n";
    out << "=========================\n\n";
    out << "1. Place Order\n";
    out << "2. View Kitchen Status\n";
    out << "3. Complete Item / Advance Time\n";
    out << "4. Show Shift Report\n";
    out << "5. Exit\n\n";
}

void MainMenu::run(std::istream& in, std::ostream& out) {
    bool running = true;
    while (running) {
        displayMenu(out);
        int choice = InputUtils::readInt("Enter choice: ", 1, 5, in, out);

        switch (choice) {
            case 1:
                if (clock_) clock_->advanceMinutes(2.0);
                orderCli_->display(in, out);
                break;
            case 2:
                if (clock_) clock_->advanceMinutes(3.0);
                kitchenStatusCli_->display(out);
                break;
            case 3:
                completionCli_->display(in, out);
                break;
            case 4:
                reportCli_->display(out);
                break;
            case 5:
                running = false;
                out << "\nExiting QuickServe Kitchen System. Goodbye!\n";
                break;
            default:
                out << "Invalid choice. Please select from the menu.\n";
                break;
        }
    }
}

} // namespace QuickServe
