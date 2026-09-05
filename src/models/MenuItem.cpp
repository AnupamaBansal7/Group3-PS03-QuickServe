#include "models/MenuItem.h"

namespace QuickServe {

MenuItem::MenuItem(int itemId, const std::string& name, StationType stationType, int slaMinutes)
    : itemId_(itemId), name_(name), stationType_(stationType), slaMinutes_(slaMinutes) {}

int MenuItem::getItemId() const {
    return itemId_;
}

const std::string& MenuItem::getName() const {
    return name_;
}

StationType MenuItem::getStationType() const {
    return stationType_;
}

int MenuItem::getSlaMinutes() const {
    return slaMinutes_;
}

} // namespace QuickServe
