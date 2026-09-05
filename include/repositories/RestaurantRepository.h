#ifndef QUICKSERVE_REPOSITORIES_RESTAURANTREPOSITORY_H
#define QUICKSERVE_REPOSITORIES_RESTAURANTREPOSITORY_H

#include "models/Restaurant.h"
#include <vector>
#include <map>
#include <memory>

namespace QuickServe {

class RestaurantRepository {
public:
    void add(const Restaurant& restaurant);
    const Restaurant* findById(int id) const;
    Restaurant* findByIdMut(int id);
    std::vector<Restaurant> getAll() const;

private:
    std::map<int, Restaurant> restaurants_;
};

} // namespace QuickServe

#endif // QUICKSERVE_REPOSITORIES_RESTAURANTREPOSITORY_H
