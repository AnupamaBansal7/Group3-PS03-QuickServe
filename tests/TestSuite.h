#ifndef QUICKSERVE_TESTS_TESTSUITE_H
#define QUICKSERVE_TESTS_TESTSUITE_H

#include <iostream>

namespace QuickServe {

class TestSuite {
public:
    static bool runAllTests(std::ostream& out = std::cout);

private:
    static bool testOrderValidation(std::ostream& out);
    static bool testStationAssignmentAndRouting(std::ostream& out);
    static bool testQueueingWhenBusy(std::ostream& out);
    static bool testFifoQueueBehavior(std::ostream& out);
    static bool testSlaPriorityInQueue(std::ostream& out);
    static bool testItemCompletionAndAutoAssignment(std::ostream& out);
    static bool testSlaBreachDetection(std::ostream& out);
    static bool testUtilizationCalculation(std::ostream& out);
    static bool testMultiFacilitySupport(std::ostream& out);
};

} // namespace QuickServe

#endif // QUICKSERVE_TESTS_TESTSUITE_H
