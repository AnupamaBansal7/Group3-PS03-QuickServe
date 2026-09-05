#ifndef QUICKSERVE_CONFIG_DATAINITIALIZER_H
#define QUICKSERVE_CONFIG_DATAINITIALIZER_H

#include "repositories/FacilityRepository.h"
#include "repositories/RestaurantRepository.h"
#include <memory>

namespace QuickServe {

class DataInitializer {
public:
    static void initialize(FacilityRepository& facilityRepo,
                           RestaurantRepository& restaurantRepo);

private:
    static void initializeRestaurants(RestaurantRepository& restaurantRepo);
    static void initializeFacilities(FacilityRepository& facilityRepo,
                                     const RestaurantRepository& restaurantRepo);
};

} // namespace QuickServe

#endif // QUICKSERVE_CONFIG_DATAINITIALIZER_H
