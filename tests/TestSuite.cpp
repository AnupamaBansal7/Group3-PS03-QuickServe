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
    out << "\nQuickServe Test Suite\n\n";
    out << "Running tests:\n";

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

    out << "\n";
    if (allPassed) {
        out << "All 9 tests passed.\n\n";
    } else {
        out << "Some tests failed.\n\n";
    }

    return allPassed;
}

bool TestSuite::testOrderValidation(std::ostream& out) {
    out << "  Test 1: Order validation and rejection logic ... ";

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

    auto resValid = orderService->createAndRouteOrder(1, {101, 103, 104});
    assert(resValid.success == true);
    assert(resValid.order != nullptr);
    assert(resValid.order->getOrderItems().size() == 3);

    auto resInvalid = orderService->createAndRouteOrder(1, {101, 999});
    assert(resInvalid.success == false);
    assert(resInvalid.order == nullptr);
    assert(resInvalid.errorMessage.find("999") != std::string::npos);

    auto resUnknownRest = orderService->createAndRouteOrder(99, {101});
    assert(resUnknownRest.success == false);
    assert(resUnknownRest.order == nullptr);

    out << "passed\n";
    return true;
}

bool TestSuite::testStationAssignmentAndRouting(std::ostream& out) {
    out << "  Test 2: Station routing and equipment specialization ... ";

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

    auto res = orderService->createAndRouteOrder(1, {101, 103, 104});
    assert(res.success);
    assert(res.routingResults.size() == 3);

    assert(res.routingResults[0].assignedToStation == true);
    assert(res.routingResults[0].assignedStationId == "T1");
    assert(res.routingResults[0].requiredStationType == StationType::TANDOOR);

    assert(res.routingResults[1].assignedToStation == true);
    assert(res.routingResults[1].assignedStationId == "F1");
    assert(res.routingResults[1].requiredStationType == StationType::FRYER);

    assert(res.routingResults[2].assignedToStation == true);
    assert(res.routingResults[2].assignedStationId == "C1");
    assert(res.routingResults[2].requiredStationType == StationType::COLD_PREP);

    out << "passed\n";
    return true;
}

bool TestSuite::testQueueingWhenBusy(std::ostream& out) {
    out << "  Test 3: Busy station queueing ... ";

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

    auto ord1 = orderService->createAndRouteOrder(1, {101, 102});
    assert(ord1.routingResults[0].assignedStationId == "T1");
    assert(ord1.routingResults[1].assignedStationId == "T2");

    auto ord2 = orderService->createAndRouteOrder(1, {101});
    assert(ord2.routingResults[0].assignedToStation == false);
    assert(ord2.routingResults[0].item->getStatus() == OrderItemStatus::QUEUED);
    assert(queueService->getQueueSize(StationType::TANDOOR) == 1);

    out << "passed\n";
    return true;
}

bool TestSuite::testFifoQueueBehavior(std::ostream& out) {
    out << "  Test 4: FIFO queue arrival order ... ";

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

    auto popped1 = queueService->getNextItem(StationType::GRILL);
    assert(popped1->getOrderItemId() == "ITM-1");

    auto popped2 = queueService->getNextItem(StationType::GRILL);
    assert(popped2->getOrderItemId() == "ITM-2");

    assert(queueService->getQueueSize(StationType::GRILL) == 0);

    out << "passed\n";
    return true;
}

bool TestSuite::testSlaPriorityInQueue(std::ostream& out) {
    out << "  Test 5: Priority queue escalation for SLA breach ... ";

    auto clock = std::make_shared<ControllableClock>();
    auto slaService = std::make_shared<SlaService>(clock);
    auto queueService = std::make_shared<QueueService>(clock, slaService);

    MenuItem miA(101, "Slow Dish", StationType::GRILL, 15);
    auto t0 = clock->now();
    auto itemA = std::make_shared<OrderItem>("ITM-A", "ORD-A", miA, t0);
    queueService->enqueue(itemA);

    clock->advanceMinutes(2.0);
    MenuItem miB(102, "Quick Slider", StationType::GRILL, 4);
    auto t1 = clock->now();
    auto itemB = std::make_shared<OrderItem>("ITM-B", "ORD-B", miB, t1);
    queueService->enqueue(itemB);

    clock->advanceMinutes(5.0);

    assert(slaService->isQueuedItemBreached(*itemA, clock->now()) == false);
    assert(slaService->isQueuedItemBreached(*itemB, clock->now()) == true);

    auto firstDispatched = queueService->getNextItem(StationType::GRILL);
    assert(firstDispatched->getOrderItemId() == "ITM-B");

    auto secondDispatched = queueService->getNextItem(StationType::GRILL);
    assert(secondDispatched->getOrderItemId() == "ITM-A");

    out << "passed\n";
    return true;
}

bool TestSuite::testItemCompletionAndAutoAssignment(std::ostream& out) {
    out << "  Test 6: Station completion and automatic assignment ... ";

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

    orderService->createAndRouteOrder(1, {101, 102});

    orderService->createAndRouteOrder(1, {101});
    assert(queueService->getQueueSize(StationType::TANDOOR) == 1);

    clock->advanceMinutes(10.0);

    auto compRes = stationService->completeStationItem("T1");
    assert(compRes.success == true);
    assert(compRes.completedItem != nullptr);
    assert(compRes.actualPrepMinutes == 10.0);
    assert(compRes.slaStatus == SlaStatus::WITHIN_SLA);

    assert(compRes.nextAssignedItem != nullptr);
    assert(queueService->getQueueSize(StationType::TANDOOR) == 0);
    assert(stationService->findStation("T1")->isBusy() == true);

    out << "passed\n";
    return true;
}

bool TestSuite::testSlaBreachDetection(std::ostream& out) {
    out << "  Test 7: SLA breach evaluation ... ";

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

    orderService->createAndRouteOrder(1, {103});
    auto f1 = stationService->findStation("F1");
    assert(f1->getSlaBreachCount() == 0);

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

    out << "passed\n";
    return true;
}

bool TestSuite::testUtilizationCalculation(std::ostream& out) {
    out << "  Test 8: Shift utilization calculation ... ";

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

    orderService->createAndRouteOrder(2, {201});
    clock->advanceMinutes(30.0);

    auto g1 = stationService->findStation("G1");
    double util = g1->getUtilization(60.0, clock->now());
    assert(util == 50.0);

    out << "passed\n";
    return true;
}

bool TestSuite::testMultiFacilitySupport(std::ostream& out) {
    out << "  Test 9: Multiple facilities isolation ... ";

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

    out << "passed\n";
    return true;
}

}
