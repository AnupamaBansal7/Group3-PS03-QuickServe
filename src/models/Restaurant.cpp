#include "models/Restaurant.h"

namespace QuickServe {

Restaurant::Restaurant(int restaurantId, const std::string& name)
    : restaurantId_(restaurantId), name_(name) {}

int Restaurant::getRestaurantId() const {
    return restaurantId_;
}

const std::string& Restaurant::getName() const {
    return name_;
}

void Restaurant::addMenuItem(const MenuItem& item) {
    menu_.push_back(item);
}

const MenuItem* Restaurant::findMenuItem(int itemId) const {
    for (const auto& item : menu_) {
        if (item.getItemId() == itemId) {
            return &item;
        }
    }
    return nullptr;
}

bool Restaurant::hasMenuItem(int itemId) const {
    return findMenuItem(itemId) != nullptr;
}

const std::vector<MenuItem>& Restaurant::getMenu() const {
    return menu_;
}

} // namespace QuickServe
