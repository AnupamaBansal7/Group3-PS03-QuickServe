CXX = g++
CXXFLAGS = -std=c++14 -O2 -Wall -Wextra -Iinclude -Itests -I.

SRC = src/models/Enums.cpp \
      src/models/MenuItem.cpp \
      src/models/Restaurant.cpp \
      src/models/OrderItem.cpp \
      src/models/Order.cpp \
      src/models/Station.cpp \
      src/models/Facility.cpp \
      src/repositories/RestaurantRepository.cpp \
      src/repositories/FacilityRepository.cpp \
      src/repositories/OrderRepository.cpp \
      src/services/SlaService.cpp \
      src/services/QueuePriorityStrategy.cpp \
      src/services/QueueService.cpp \
      src/services/StationService.cpp \
      src/services/OrderRoutingService.cpp \
      src/services/OrderService.cpp \
      src/services/ReportService.cpp \
      src/config/DataInitializer.cpp \
      src/cli/OrderCLI.cpp \
      src/cli/KitchenStatusCLI.cpp \
      src/cli/CompletionCLI.cpp \
      src/cli/ReportCLI.cpp \
      src/cli/MainMenu.cpp \
      src/utils/IdGenerator.cpp \
      src/utils/InputUtils.cpp \
      src/utils/TimeUtils.cpp \
      tests/TestSuite.cpp \
      src/main.cpp

TARGET = quickserve.exe

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	del $(TARGET) 2>nul || rm -f $(TARGET)

test: $(TARGET)
	./$(TARGET) --test

.PHONY: all clean test
