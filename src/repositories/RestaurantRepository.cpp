#include "repositories/RestaurantRepository.h"

namespace QuickServe {

void RestaurantRepository::add(const Restaurant& restaurant) {
    restaurants_.insert({restaurant.getRestaurantId(), restaurant});
}

const Restaurant* RestaurantRepository::findById(int id) const {
    auto it = restaurants_.find(id);
    if (it != restaurants_.end()) {
        return &(it->second);
    }
    return nullptr;
}

Restaurant* RestaurantRepository::findByIdMut(int id) {
    auto it = restaurants_.find(id);
    if (it != restaurants_.end()) {
        return &(it->second);
    }
    return nullptr;
}

std::vector<Restaurant> RestaurantRepository::getAll() const {
    std::vector<Restaurant> list;
    for (const auto& pair : restaurants_) {
        list.push_back(pair.second);
    }
    return list;
}

} // namespace QuickServe
