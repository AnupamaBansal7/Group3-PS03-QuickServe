#ifndef QUICKSERVE_MODELS_RESTAURANT_H
#define QUICKSERVE_MODELS_RESTAURANT_H

#include "models/MenuItem.h"
#include <string>
#include <vector>

namespace QuickServe {

class Restaurant {
public:
    Restaurant(int restaurantId, const std::string& name);

    int getRestaurantId() const;
    const std::string& getName() const;

    void addMenuItem(const MenuItem& item);
    const MenuItem* findMenuItem(int itemId) const;
    bool hasMenuItem(int itemId) const;
    const std::vector<MenuItem>& getMenu() const;

private:
    int restaurantId_;
    std::string name_;
    std::vector<MenuItem> menu_;
};

} // namespace QuickServe

#endif // QUICKSERVE_MODELS_RESTAURANT_H
