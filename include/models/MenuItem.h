#ifndef QUICKSERVE_MODELS_MENUITEM_H
#define QUICKSERVE_MODELS_MENUITEM_H

#include "models/Enums.h"
#include <string>

namespace QuickServe {

class MenuItem {
public:
    MenuItem(int itemId, const std::string& name, StationType stationType, int slaMinutes);

    int getItemId() const;
    const std::string& getName() const;
    StationType getStationType() const;
    int getSlaMinutes() const;

private:
    int itemId_;
    std::string name_;
    StationType stationType_;
    int slaMinutes_;
};

} // namespace QuickServe

#endif // QUICKSERVE_MODELS_MENUITEM_H
