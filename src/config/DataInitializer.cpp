#include "config/DataInitializer.h"
#include "models/Station.h"

namespace QuickServe {

void DataInitializer::initialize(FacilityRepository& facilityRepo,
                                 RestaurantRepository& restaurantRepo) {
    initializeRestaurants(restaurantRepo);
    initializeFacilities(facilityRepo, restaurantRepo);
}

void DataInitializer::initializeRestaurants(RestaurantRepository& restaurantRepo) {
    // Restaurant 1: Spice Route (ID: 1)
    Restaurant spiceRoute(1, "Spice Route");
    spiceRoute.addMenuItem(MenuItem(101, "Chicken Tikka",  StationType::TANDOOR,   15));
    spiceRoute.addMenuItem(MenuItem(102, "Paneer Tikka",   StationType::TANDOOR,   12));
    spiceRoute.addMenuItem(MenuItem(103, "French Fries",   StationType::FRYER,      8));
    spiceRoute.addMenuItem(MenuItem(104, "Green Salad",    StationType::COLD_PREP,  5));
    restaurantRepo.add(spiceRoute);

    // Restaurant 2: Burger Bay (ID: 2)
    Restaurant burgerBay(2, "Burger Bay");
    burgerBay.addMenuItem(MenuItem(201, "Classic Burger",  StationType::GRILL,     12));
    burgerBay.addMenuItem(MenuItem(202, "Cheese Burger",   StationType::GRILL,     14));
    burgerBay.addMenuItem(MenuItem(203, "Onion Rings",     StationType::FRYER,      6));
    restaurantRepo.add(burgerBay);

    // Restaurant 3: Green Bowl (ID: 3)
    Restaurant greenBowl(3, "Green Bowl");
    greenBowl.addMenuItem(MenuItem(301, "Caesar Salad",        StationType::COLD_PREP, 5));
    greenBowl.addMenuItem(MenuItem(302, "Quinoa Bowl",         StationType::COLD_PREP, 7));
    greenBowl.addMenuItem(MenuItem(303, "Grilled Veggie Wrap", StationType::GRILL,    10));
    restaurantRepo.add(greenBowl);
}

void DataInitializer::initializeFacilities(FacilityRepository& facilityRepo,
                                          const RestaurantRepository& /*restaurantRepo*/) {
    // Facility 1: Central Kitchen (Primary Facility)
    auto centralKitchen = std::make_shared<Facility>("FAC-1", "Central Kitchen");

    // Configure Stations: TANDOOR (T1, T2), FRYER (F1, F2), GRILL (G1, G2), COLD_PREP (C1, C2)
    centralKitchen->addStation(std::make_shared<Station>("T1", StationType::TANDOOR));
    centralKitchen->addStation(std::make_shared<Station>("T2", StationType::TANDOOR));
    centralKitchen->addStation(std::make_shared<Station>("F1", StationType::FRYER));
    centralKitchen->addStation(std::make_shared<Station>("F2", StationType::FRYER));
    centralKitchen->addStation(std::make_shared<Station>("G1", StationType::GRILL));
    centralKitchen->addStation(std::make_shared<Station>("G2", StationType::GRILL));
    centralKitchen->addStation(std::make_shared<Station>("C1", StationType::COLD_PREP));
    centralKitchen->addStation(std::make_shared<Station>("C2", StationType::COLD_PREP));

    // Register operating restaurants at Central Kitchen
    centralKitchen->addRestaurantId(1); // Spice Route
    centralKitchen->addRestaurantId(2); // Burger Bay
    centralKitchen->addRestaurantId(3); // Green Bowl

    facilityRepo.add(centralKitchen);
    facilityRepo.setActiveFacilityId("FAC-1");

    // Facility 2: Downtown Express (Demonstrating multi-facility support)
    auto downtownExpress = std::make_shared<Facility>("FAC-2", "Downtown Express");
    downtownExpress->addStation(std::make_shared<Station>("DT-G1", StationType::GRILL));
    downtownExpress->addStation(std::make_shared<Station>("DT-F1", StationType::FRYER));
    downtownExpress->addStation(std::make_shared<Station>("DT-C1", StationType::COLD_PREP));
    downtownExpress->addRestaurantId(2); // Burger Bay
    downtownExpress->addRestaurantId(3); // Green Bowl

    facilityRepo.add(downtownExpress);
}

} // namespace QuickServe
