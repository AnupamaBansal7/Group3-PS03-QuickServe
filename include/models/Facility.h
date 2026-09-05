#ifndef QUICKSERVE_MODELS_FACILITY_H
#define QUICKSERVE_MODELS_FACILITY_H

#include "models/Station.h"
#include <string>
#include <vector>
#include <memory>
#include <set>

namespace QuickServe {

class Facility {
public:
    Facility(const std::string& facilityId, const std::string& name);

    const std::string& getFacilityId() const;
    const std::string& getName() const;

    void addStation(std::shared_ptr<Station> station);
    std::shared_ptr<Station> findStation(const std::string& stationId) const;
    const std::vector<std::shared_ptr<Station>>& getAllStations() const;
    std::vector<std::shared_ptr<Station>> getStationsByType(StationType type) const;
    std::shared_ptr<Station> findFreeStation(StationType type) const;

    int getTotalStationCount(StationType type) const;
    int getFreeStationCount(StationType type) const;
    int getBusyStationCount(StationType type) const;

    void addRestaurantId(int restaurantId);
    bool hasRestaurantId(int restaurantId) const;
    const std::set<int>& getRestaurantIds() const;

private:
    std::string facilityId_;
    std::string name_;
    std::vector<std::shared_ptr<Station>> stations_;
    std::set<int> restaurantIds_;
};

} // namespace QuickServe

#endif // QUICKSERVE_MODELS_FACILITY_H
