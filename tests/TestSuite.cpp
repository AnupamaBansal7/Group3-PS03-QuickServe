#include "TestSuite.h"
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
#include "utils/IdGenerator.h"
#include <cassert>
#include <iomanip>

namespace QuickServe {

bool TestSuite::runAllTests(std::ostream& out) {
    out << "\n======================================================================\n";
    out << "            QUICKSERVE FOODS - INDEPENDENT SERVICE TEST SUITE         \n";
    out << "  Verifying Domain Services, Queue Logic, SLAs & Architecture Layering\n";
    out << "======================================================================\n\n";

    bool allPassed = true;

    allPassed &= testOrderValidation(out);
    allPassed &= testStationAssignmentAndRouting(out);
    allPassed &= testQueueingWhenBusy(out);
    allPassed &= testFifoQueueBehavior(out);
    allPassed &= testSlaPriorityInQueue(out);
    allPassed &= testItemCompletionAndAutoAssignment(out);
    allPassed &= testSlaBreachDetection(out);
    allPassed &= testUtilizationCalculation(out);
    allPassed &= testMultiFacilitySupport(out);

    out << "\n======================================================================\n";
    if (allPassed) {
        out << "        >>> ALL INDEPENDENT SERVICE TESTS PASSED! (9/9) <<<\n";
    } else {
        out << "        >>> SOME SERVICE TESTS FAILED! <<<\n";
    }
    out << "======================================================================\n\n";

    return allPassed;
}

bool TestSuite::testOrderValidation(std::ostream& out) {
    out << "[TEST 1] Order Validation & Rejection Logic... ";

    auto facilityRepo = std::make_shared<FacilityRepository>();
    auto restRepo = std::make_shared<RestaurantRepository>();
    auto orderRepo = std::make_shared<OrderRepository>();
    DataInitializer::initialize(*facilityRepo, *restRepo);

    auto clock = std::make_shared<ControllableClock>();
    auto slaService = std::make_shared<SlaService>(clock);
    auto queueService = std::make_shared<QueueService>(clock, slaService);
    auto stationService = std::make_shared<StationService>(facilityRepo, clock, slaService, queueService);
    auto routingService = std::make_shared<OrderRoutingService>(stationService, queueService, clock);
    auto idGen = std::make_shared<IdGenerator>();
    auto orderService = std::make_shared<OrderService>(restRepo, orderRepo, routingService, clock, idGen);

    // 1. Valid items for Spice Route (101, 103, 104)
    auto resValid = orderService->createAndRouteOrder(1, {101, 103, 104});
    assert(resValid.success == true);
    assert(resValid.order != nullptr);
    assert(resValid.order->getOrderItems().size() == 3);

    // 2. Invalid item ID 999
    auto resInvalid = orderService->createAndRouteOrder(1, {101, 999});
    assert(resInvalid.success == false);
    assert(resInvalid.order == nullptr);
    assert(resInvalid.errorMessage.find("999") != std::string::npos);

    // 3. Unknown restaurant ID 99
    auto resUnknownRest = orderService->createAndRouteOrder(99, {101});
    assert(resUnknownRest.success == false);
    assert(resUnknownRest.order == nullptr);

    out << "PASSED\n";
    return true;
}

bool TestSuite::testStationAssignmentAndRouting(std::ostream& out) {
    out << "[TEST 2] Station Type Routing & Equipment Specialization... ";

    auto facilityRepo = std::make_shared<FacilityRepository>();
    auto restRepo = std::make_shared<RestaurantRepository>();
    auto orderRepo = std::make_shared<OrderRepository>();
    DataInitializer::initialize(*facilityRepo, *restRepo);

    auto clock = std::make_shared<ControllableClock>();
    auto slaService = std::make_shared<SlaService>(clock);
    auto queueService = std::make_shared<QueueService>(clock, slaService);
    auto stationService = std::make_shared<StationService>(facilityRepo, clock, slaService, queueService);
    auto routingService = std::make_shared<OrderRoutingService>(stationService, queueService, clock);
    auto idGen = std::make_shared<IdGenerator>();
    auto orderService = std::make_shared<OrderService>(restRepo, orderRepo, routingService, clock, idGen);

    // Order with TANDOOR (101), FRYER (103), and COLD_PREP (104)
    auto res = orderService->createAndRouteOrder(1, {101, 103, 104});
    assert(res.success);
    assert(res.routingResults.size() == 3);

    // 101 -> T1 [TANDOOR]
    assert(res.routingResults[0].assignedToStation == true);
    assert(res.routingResults[0].assignedStationId == "T1");
    assert(res.routingResults[0].requiredStationType == StationType::TANDOOR);

    // 103 -> F1 [FRYER]
    assert(res.routingResults[1].assignedToStation == true);
    assert(res.routingResults[1].assignedStationId == "F1");
    assert(res.routingResults[1].requiredStationType == StationType::FRYER);

    // 104 -> C1 [COLD_PREP]
    assert(res.routingResults[2].assignedToStation == true);
    assert(res.routingResults[2].assignedStationId == "C1");
    assert(res.routingResults[2].requiredStationType == StationType::COLD_PREP);

    out << "PASSED\n";
    return true;
}

bool TestSuite::testQueueingWhenBusy(std::ostream& out) {
    out << "[TEST 3] Busy Station Queueing (No Dropped Items)... ";

    auto facilityRepo = std::make_shared<FacilityRepository>();
    auto restRepo = std::make_shared<RestaurantRepository>();
    auto orderRepo = std::make_shared<OrderRepository>();
    DataInitializer::initialize(*facilityRepo, *restRepo);

    auto clock = std::make_shared<ControllableClock>();
    auto slaService = std::make_shared<SlaService>(clock);
    auto queueService = std::make_shared<QueueService>(clock, slaService);
    auto stationService = std::make_shared<StationService>(facilityRepo, clock, slaService, queueService);
    auto routingService = std::make_shared<OrderRoutingService>(stationService, queueService, clock);
    auto idGen = std::make_shared<IdGenerator>();
    auto orderService = std::make_shared<OrderService>(restRepo, orderRepo, routingService, clock, idGen);

    // Central Kitchen has 2 Tandoors: T1, T2
    // Order 1 takes T1 (101) and T2 (102)
    auto ord1 = orderService->createAndRouteOrder(1, {101, 102});
    assert(ord1.routingResults[0].assignedStationId == "T1");
    assert(ord1.routingResults[1].assignedStationId == "T2");

    // Order 2 requests another Tandoor item (101)
    auto ord2 = orderService->createAndRouteOrder(1, {101});
    assert(ord2.routingResults[0].assignedToStation == false); // Queued!
    assert(ord2.routingResults[0].item->getStatus() == OrderItemStatus::QUEUED);
    assert(queueService->getQueueSize(StationType::TANDOOR) == 1);

    out << "PASSED\n";
    return true;
}

bool TestSuite::testFifoQueueBehavior(std::ostream& out) {
    out << "[TEST 4] Standard Arrival Order FIFO Queueing... ";

    auto clock = std::make_shared<ControllableClock>();
    auto slaService = std::make_shared<SlaService>(clock);
    auto queueService = std::make_shared<QueueService>(clock, slaService);

    MenuItem mi1(101, "Item A", StationType::GRILL, 15);
    MenuItem mi2(102, "Item B", StationType::GRILL, 15);

    auto t0 = clock->now();
    auto itemA = std::make_shared<OrderItem>("ITM-1", "ORD-1", mi1, t0);

    clock->advanceMinutes(1.0);
    auto t1 = clock->now();
    auto itemB = std::make_shared<OrderItem>("ITM-2", "ORD-2", mi2, t1);

    queueService->enqueue(itemA);
    queueService->enqueue(itemB);

    assert(queueService->getQueueSize(StationType::GRILL) == 2);

    // Neither is breaching, pop must return Item A first, then Item B
    auto popped1 = queueService->getNextItem(StationType::GRILL);
    assert(popped1->getOrderItemId() == "ITM-1");

    auto popped2 = queueService->getNextItem(StationType::GRILL);
    assert(popped2->getOrderItemId() == "ITM-2");

    assert(queueService->getQueueSize(StationType::GRILL) == 0);

    out << "PASSED\n";
    return true;
}

bool TestSuite::testSlaPriorityInQueue(std::ostream& out) {
    out << "[TEST 5] SLA-Breaching Queued Item Priority Escalation... ";

    auto clock = std::make_shared<ControllableClock>();
    auto slaService = std::make_shared<SlaService>(clock);
    auto queueService = std::make_shared<QueueService>(clock, slaService);

    // Item A has target SLA 15 min, arrives at T0
    MenuItem miA(101, "Slow Dish", StationType::GRILL, 15);
    auto t0 = clock->now();
    auto itemA = std::make_shared<OrderItem>("ITM-A", "ORD-A", miA, t0);
    queueService->enqueue(itemA);

    // Item B has target SLA 4 min, arrives at T0 + 2m
    clock->advanceMinutes(2.0);
    MenuItem miB(102, "Quick Slider", StationType::GRILL, 4);
    auto t1 = clock->now();
    auto itemB = std::make_shared<OrderItem>("ITM-B", "ORD-B", miB, t1);
    queueService->enqueue(itemB);

    // Advance clock to T0 + 7.0m:
    // Item A waited 7m (SLA: 15m) -> NOT breached
    // Item B waited 5m (SLA: 4m)  -> BREACHED in queue!
    clock->advanceMinutes(5.0);

    // Verify queueService detects Item B breach and elevates it ahead of Item A!
    assert(slaService->isQueuedItemBreached(*itemA, clock->now()) == false);
    assert(slaService->isQueuedItemBreached(*itemB, clock->now()) == true);

    auto firstDispatched = queueService->getNextItem(StationType::GRILL);
    assert(firstDispatched->getOrderItemId() == "ITM-B"); // Item B jumped ahead!

    auto secondDispatched = queueService->getNextItem(StationType::GRILL);
    assert(secondDispatched->getOrderItemId() == "ITM-A"); // Item A dispatched second

    out << "PASSED\n";
    return true;
}

bool TestSuite::testItemCompletionAndAutoAssignment(std::ostream& out) {
    out << "[TEST 6] Station Completion & Automatic Queued Assignment... ";

    auto facilityRepo = std::make_shared<FacilityRepository>();
    auto restRepo = std::make_shared<RestaurantRepository>();
    auto orderRepo = std::make_shared<OrderRepository>();
    DataInitializer::initialize(*facilityRepo, *restRepo);

    auto clock = std::make_shared<ControllableClock>();
    auto slaService = std::make_shared<SlaService>(clock);
    auto queueService = std::make_shared<QueueService>(clock, slaService);
    auto stationService = std::make_shared<StationService>(facilityRepo, clock, slaService, queueService);
    auto routingService = std::make_shared<OrderRoutingService>(stationService, queueService, clock);
    auto idGen = std::make_shared<IdGenerator>();
    auto orderService = std::make_shared<OrderService>(restRepo, orderRepo, routingService, clock, idGen);

    // Fill both Tandoors T1 and T2
    orderService->createAndRouteOrder(1, {101, 102});

    // Queue item 101 into Tandoor queue
    orderService->createAndRouteOrder(1, {101});
    assert(queueService->getQueueSize(StationType::TANDOOR) == 1);

    // Advance clock by 10 minutes
    clock->advanceMinutes(10.0);

    // Complete T1
    auto compRes = stationService->completeStationItem("T1");
    assert(compRes.success == true);
    assert(compRes.completedItem != nullptr);
    assert(compRes.actualPrepMinutes == 10.0);
    assert(compRes.slaStatus == SlaStatus::WITHIN_SLA);

    // T1 should have automatically pulled the queued item!
    assert(compRes.nextAssignedItem != nullptr);
    assert(queueService->getQueueSize(StationType::TANDOOR) == 0);
    assert(stationService->findStation("T1")->isBusy() == true);

    out << "PASSED\n";
    return true;
}

bool TestSuite::testSlaBreachDetection(std::ostream& out) {
    out << "[TEST 7] Actual Preparation SLA Breach Evaluation... ";

    auto facilityRepo = std::make_shared<FacilityRepository>();
    auto restRepo = std::make_shared<RestaurantRepository>();
    auto orderRepo = std::make_shared<OrderRepository>();
    DataInitializer::initialize(*facilityRepo, *restRepo);

    auto clock = std::make_shared<ControllableClock>();
    auto slaService = std::make_shared<SlaService>(clock);
    auto queueService = std::make_shared<QueueService>(clock, slaService);
    auto stationService = std::make_shared<StationService>(facilityRepo, clock, slaService, queueService);
    auto routingService = std::make_shared<OrderRoutingService>(stationService, queueService, clock);
    auto idGen = std::make_shared<IdGenerator>();
    auto orderService = std::make_shared<OrderService>(restRepo, orderRepo, routingService, clock, idGen);

    // Item 103 (French Fries, FRYER, SLA: 8 min)
    orderService->createAndRouteOrder(1, {103}); // Starts on F1
    auto f1 = stationService->findStation("F1");
    assert(f1->getSlaBreachCount() == 0);

    // Cook for 12 minutes (exceeds 8 min SLA)
    clock->advanceMinutes(12.0);

    auto compRes = stationService->completeStationItem("F1");
    assert(compRes.success == true);
    assert(compRes.actualPrepMinutes == 12.0);
    assert(compRes.slaStatus == SlaStatus::BREACHED);
    assert(compRes.isBreached() == true);
    assert(compRes.completedItem != nullptr);
    assert(compRes.completedItem->isSlaBreached() == true);
    assert(compRes.completedItem->hasBreachedSla() == true);
    assert(compRes.completedItem->getActualPrepMinutes() == 12.0);
    assert(f1->getSlaBreachCount() == 1);

    out << "PASSED\n";
    return true;
}

bool TestSuite::testUtilizationCalculation(std::ostream& out) {
    out << "[TEST 8] Shift Utilization Calculation & Active Time Inclusion... ";

    auto facilityRepo = std::make_shared<FacilityRepository>();
    auto restRepo = std::make_shared<RestaurantRepository>();
    auto orderRepo = std::make_shared<OrderRepository>();
    DataInitializer::initialize(*facilityRepo, *restRepo);

    auto clock = std::make_shared<ControllableClock>();
    auto slaService = std::make_shared<SlaService>(clock);
    auto queueService = std::make_shared<QueueService>(clock, slaService);
    auto stationService = std::make_shared<StationService>(facilityRepo, clock, slaService, queueService);
    auto routingService = std::make_shared<OrderRoutingService>(stationService, queueService, clock);
    auto idGen = std::make_shared<IdGenerator>();
    auto orderService = std::make_shared<OrderService>(restRepo, orderRepo, routingService, clock, idGen);
    auto reportService = std::make_shared<ReportService>(facilityRepo, orderRepo, queueService, clock, 60.0);

    // Item 201 on G1 (GRILL)
    orderService->createAndRouteOrder(2, {201});
    clock->advanceMinutes(30.0);

    // G1 has been cooking for 30 minutes out of a 60 min shift -> 50% utilization
    auto g1 = stationService->findStation("G1");
    double util = g1->getUtilization(60.0, clock->now());
    assert(util == 50.0);

    out << "PASSED\n";
    return true;
}

bool TestSuite::testMultiFacilitySupport(std::ostream& out) {
    out << "[TEST 9] Multiple Facilities Support & Isolation... ";

    auto facilityRepo = std::make_shared<FacilityRepository>();
    auto restRepo = std::make_shared<RestaurantRepository>();
    DataInitializer::initialize(*facilityRepo, *restRepo);

    assert(facilityRepo->getAll().size() == 2);
    auto fac1 = facilityRepo->findById("FAC-1");
    auto fac2 = facilityRepo->findById("FAC-2");

    assert(fac1 != nullptr && fac1->getName() == "Central Kitchen");
    assert(fac2 != nullptr && fac2->getName() == "Downtown Express");

    assert(fac1->getAllStations().size() == 8);
    assert(fac2->getAllStations().size() == 3);

    out << "PASSED\n";
    return true;
}

} // namespace QuickServe
