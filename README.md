# QuickServe Foods

A C++17 application that manages cloud kitchen orders and cooking stations.

### Features

* Place and validate orders.
* Assign orders to cooking stations.
* Manage queues using SLA priority.
* Track station status and utilization.
* Generate shift reports.
* Support multiple facilities.

### Tech Stack

* C++17
* CMake / Make
* Standard C++ Library

### Build & Run

```bash
make
quickserve.exe
```

### Run Tests

```bash
quickserve.exe --test
```

### Project Structure

* `include/` — Header files
* `src/` — Source code
* `tests/` — Unit tests
* `config/` — Kitchen data
