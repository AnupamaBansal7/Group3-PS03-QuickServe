# QuickServe Foods – Order Routing & Station Allocation System

A clean, modular C++ application modeling a multi-brand cloud kitchen facility. The system manages specialized cooking stations, restaurant menus, order intake validation, dynamic SLA-aware queue prioritization, deterministic station assignment, and shift utilization analytics.

---

## 1. Project Architecture & Layering

The codebase strictly adheres to **Separation of Concerns**, cleanly dividing models, in-memory repositories, domain services, hardcoded configuration, utilities, and CLI presentation:

```
QuickServe/
├── CMakeLists.txt              # Standard CMake build configuration (C++17)
├── Makefile                    # Makefile for GCC / MinGW / Linux
├── build.bat                   # Windows batch build script
├── run.bat                     # Windows batch runner script
├── include/
│   ├── models/
│   │   ├── Enums.h             # StationType, StationStatus, OrderItemStatus, SlaStatus
│   │   ├── MenuItem.h          # Item ID, name, station type, target SLA minutes
│   │   ├── Restaurant.h        # Restaurant entity and menu container
│   │   ├── Station.h           # Physical station, state, busy time, SLA breach count
│   │   ├── OrderItem.h         # Item lifecycle, chrono timestamps, SLA status
│   │   ├── Order.h             # Customer order container
│   │   └── Facility.h          # Facility entity with stations and operating restaurants
│   ├── repositories/
│   │   ├── FacilityRepository.h   # In-memory storage & query for facilities
│   │   ├── RestaurantRepository.h # In-memory storage & query for restaurants
│   │   └── OrderRepository.h      # In-memory storage & query for orders
│   ├── services/
│   │   ├── IClock.h            # Clock abstraction (SystemClock & ControllableClock)
│   │   ├── SlaService.h        # SLA target comparisons, prep duration, queue breach checks
│   │   ├── QueueService.h      # Dedicated station-type queues with SLA priority rule
│   │   ├── StationService.h    # Station state, free/busy transitions, utilization metrics
│   │   ├── OrderRoutingService.h # Dispatch item to free station or enqueue
│   │   ├── OrderService.h      # Order validation, creation, lifecycle management
│   │   └── ReportService.h     # Utilization calculations and SLA compliance statistics
│   ├── config/
│   │   └── DataInitializer.h   # Dedicated hardcoded configuration initializer
│   ├── cli/
│   │   ├── MainMenu.h          # Primary CLI menu loop
│   │   ├── OrderCLI.h          # Restaurant selection, menu display, item order placement
│   │   ├── KitchenStatusCLI.h  # Station availability & individual station viewer
│   │   ├── CompletionCLI.h     # Active station completion & automatic queued assignment
│   │   └── ReportCLI.h         # Shift utilization and SLA compliance display
│   └── utils/
│       ├── IdGenerator.h       # Sequential Order ID generator (ORD-1001)
│       ├── InputUtils.h        # Robust CLI input parsing (handles invalid inputs safely)
│       └── TimeUtils.h         # std::chrono formatting and duration helpers
├── src/
│   ├── models/                 # Model implementations
│   ├── repositories/           # Repository implementations
│   ├── services/               # Service business logic implementations
│   ├── config/                 # DataInitializer implementation
│   ├── cli/                    # CLI presentation implementations
│   ├── utils/                  # Utility implementations
│   └── main.cpp                # Clean main entry point (IoC wiring & CLI starter)
└── tests/
    ├── TestSuite.h             # Standalone test suite declaration
    └── TestSuite.cpp           # Independent unit tests for all business logic
```

---

## 2. Hardcoded Configuration (`DataInitializer`)

All kitchen facilities, physical stations, restaurants, menus, and target SLAs are **hardcoded in C++ code** via `DataInitializer`. The user does NOT configure these dynamically at runtime:

### Restaurants & Menus:
1. **Spice Route** (ID: 1):
   - `101`: Chicken Tikka (`TANDOOR`, 15 min)
   - `102`: Paneer Tikka (`TANDOOR`, 12 min)
   - `103`: French Fries (`FRYER`, 8 min)
   - `104`: Green Salad (`COLD_PREP`, 5 min)
2. **Burger Bay** (ID: 2):
   - `201`: Classic Burger (`GRILL`, 12 min)
   - `202`: Cheese Burger (`GRILL`, 14 min)
   - `203`: Onion Rings (`FRYER`, 6 min)
3. **Green Bowl** (ID: 3):
   - `301`: Caesar Salad (`COLD_PREP`, 5 min)
   - `302`: Quinoa Bowl (`COLD_PREP`, 7 min)
   - `303`: Grilled Veggie Wrap (`GRILL`, 10 min)

### Physical Stations (Central Kitchen - `FAC-1`):
- `T1`, `T2`: `TANDOOR`
- `F1`, `F2`: `FRYER`
- `G1`, `G2`: `GRILL`
- `C1`, `C2`: `COLD_PREP`

### Multiple Facilities Support:
- Architecture supports multiple facilities. `FAC-2` ("Downtown Express") is also configured with its own stations (`DT-G1`, `DT-F1`, `DT-C1`) and operating restaurants.

---

## 3. Business Logic & Separation of Concerns

### Order Validation & Creation (`OrderService`)
- Validation lives strictly in the service layer, NOT in the CLI.
- Checks restaurant existence and validates every requested item ID against that restaurant's menu.
- If ANY item is invalid: rejects the entire order immediately with a clear message (e.g. `ERROR: Item ID 999 does not exist in Spice Route menu. Order rejected.`). Does not partially create the order.
- Generates unique order ID (`ORD-1001`) via `IdGenerator` and records arrival time via `IClock`.

### Station Routing (`OrderRoutingService`)
- For each item, determines required `StationType`.
- Checks if a physical station of that type is `FREE`.
  - If free: assigns item, marks station `BUSY`, records start time.
  - If all stations of that type are busy: enqueues the item in `QueueService` for that station type.

### Queue Management & SLA Priority Rule (`QueueService`)
- Maintains a separate queue for each station type (`GRILL`, `FRYER`, `COLD_PREP`, `TANDOOR`).
- **Priority Rules**:
  1. **SLA-Breaching Items**: Any item whose elapsed wait time exceeds its target SLA (`currentTime - arrivalTime > slaMinutes`) jumps ahead of normal waiting items.
  2. **Relative Arrival Order Preservation**:
     - Multiple breaching items preserve their relative arrival order.
     - Normal items strictly follow FIFO arrival order.

### Item Completion & Automatic Queued Assignment (`StationService`)
- Triggered by Station ID (e.g. `T1`).
- Records completion time using `IClock`.
- Calculates actual prep time: `completionTime - startTime`.
- Compares with item's `slaMinutes`:
  - `actualPrepTime > slaMinutes` $\implies$ `BREACHED`, increments station's SLA breach counter.
  - `actualPrepTime <= slaMinutes` $\implies$ `WITHIN SLA`.
- Updates station accumulated busy minutes.
- Frees the station and immediately checks the station type's queue via `QueueService`.
- If a queued item is eligible, **automatically assigns** it to the newly freed station.

### Station Utilization & Shift Reporting (`ReportService`)
- Calculates station utilization percentage:
  $$\text{Utilization } \% = \left(\frac{\text{Accumulated Busy Minutes} + \text{Ongoing Busy Minutes}}{\text{Shift Duration}}\right) \times 100\%$$
- Clamped to $[0, 100\%]$ to prevent overflow; safely avoids division by zero.
- Calculates SLA Compliance:
  $$\text{SLA Compliance } \% = \left(\frac{\text{Completed Items} - \text{SLA Breaches}}{\text{Completed Items}}\right) \times 100\%$$

---

## 4. Time Abstraction (`IClock`)

Time handling uses `std::chrono` through the `IClock` abstraction:
- `SystemClock`: Returns real `std::chrono::system_clock::now()`.
- `ControllableClock`: Controllable clock supporting `advanceMinutes(double)` and `setTime(time_point)`, allowing tests and SLA breach scenarios to execute deterministically without waiting for real minutes to pass.

---

## 5. Build & Run Instructions

### Prerequisites
- C++14 or C++17 compliant compiler (`g++`, `clang++`, or MSVC).
- Uses only the C++ standard library.

### Building on Windows

Using `build.bat`:
```cmd
build.bat
```

Using `make`:
```cmd
make
```

Using `CMake`:
```cmd
mkdir build && cd build
cmake ..
cmake --build .
```

### Running the Application

#### 1. Interactive CLI
```cmd
quickserve.exe
```
Presents the menu:
```text
========================================
QUICKSERVE KITCHEN SYSTEM
=========================

1. Place Order
2. View Kitchen Status
3. Complete Item
4. Show Shift Report
5. Exit

Enter choice:
```

#### 2. Running Independent Unit Test Suite
```cmd
quickserve.exe --test
```
Runs 9 comprehensive automated unit tests covering order validation, routing, queueing, FIFO preservation, queued SLA priority jumps, station completion, SLA breach calculation, utilization formulas, and multi-facility support.
