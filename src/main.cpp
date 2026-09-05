#include "repositories/FacilityRepository.h"
#include "repositories/RestaurantRepository.h"
#include "repositories/OrderRepository.h"
#include "services/IClock.h"
#include "services/SlaService.h"
#include "services/QueueService.h"
#include "services/StationService.h"
#include "services/OrderRoutingService.h"
#include "services/OrderService.h"
#include "services/ReportService.h"
#include "config/DataInitializer.h"
#include "cli/OrderCLI.h"
#include "cli/KitchenStatusCLI.h"
#include "cli/CompletionCLI.h"
#include "cli/ReportCLI.h"
#include "cli/MainMenu.h"
#include "utils/IdGenerator.h"
#include "tests/TestSuite.h"
#include <iostream>
#include <string>
#include <memory>

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--test" || arg == "--demo" || arg == "-t" || arg == "-d") {
            bool success = QuickServe::TestSuite::runAllTests(std::cout);
            return success ? 0 : 1;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "QuickServe Foods - Order Routing & Station Allocation System\n";
            std::cout << "Usage:\n";
            std::cout << "  quickserve.exe         Start interactive CLI\n";
            std::cout << "  quickserve.exe --test  Run independent automated service test suite\n";
            std::cout << "  quickserve.exe --help  Display this help message\n";
            return 0;
        }
    }

    // 1. Clock Abstraction
    auto clock = std::make_shared<QuickServe::ControllableClock>();

    // 2. Repositories
    auto facilityRepo = std::make_shared<QuickServe::FacilityRepository>();
    auto restaurantRepo = std::make_shared<QuickServe::RestaurantRepository>();
    auto orderRepo = std::make_shared<QuickServe::OrderRepository>();

    // 3. Hardcoded Configuration via DataInitializer
    QuickServe::DataInitializer::initialize(*facilityRepo, *restaurantRepo);

    // 4. Utilities
    auto idGenerator = std::make_shared<QuickServe::IdGenerator>(1001);

    // 5. Services Layer
    auto slaService = std::make_shared<QuickServe::SlaService>(clock);
    auto queueService = std::make_shared<QuickServe::QueueService>(clock, slaService);
    auto stationService = std::make_shared<QuickServe::StationService>(facilityRepo, clock, slaService, queueService);
    auto routingService = std::make_shared<QuickServe::OrderRoutingService>(stationService, queueService, clock);
    auto orderService = std::make_shared<QuickServe::OrderService>(restaurantRepo, orderRepo, routingService, clock, idGenerator);
    auto reportService = std::make_shared<QuickServe::ReportService>(facilityRepo, orderRepo, queueService, clock, stationService, 60.0);

    // 6. CLI Presentation Layer
    auto orderCli = std::make_shared<QuickServe::OrderCLI>(orderService, restaurantRepo);
    auto kitchenStatusCli = std::make_shared<QuickServe::KitchenStatusCLI>(reportService);
    auto completionCli = std::make_shared<QuickServe::CompletionCLI>(stationService, clock);
    auto reportCli = std::make_shared<QuickServe::ReportCLI>(reportService);

    // 7. Start Main CLI Loop
    QuickServe::MainMenu mainMenu(orderCli, kitchenStatusCli, completionCli, reportCli, clock);
    mainMenu.run();

    return 0;
}
