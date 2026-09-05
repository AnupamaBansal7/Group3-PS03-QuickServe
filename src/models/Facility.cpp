#include "models/Facility.h"

namespace QuickServe {

Facility::Facility(const std::string& facilityId, const std::string& name)
    : facilityId_(facilityId), name_(name) {}

const std::string& Facility::getFacilityId() const {
    return facilityId_;
}

const std::string& Facility::getName() const {
    return name_;
}

void Facility::addStation(std::shared_ptr<Station> station) {
    if (station) {
        stations_.push_back(station);
    }
}

std::shared_ptr<Station> Facility::findStation(const std::string& stationId) const {
    for (const auto& st : stations_) {
        if (st->getStationId() == stationId) {
            return st;
        }
    }
    return nullptr;
}

const std::vector<std::shared_ptr<Station>>& Facility::getAllStations() const {
    return stations_;
}

std::vector<std::shared_ptr<Station>> Facility::getStationsByType(StationType type) const {
    std::vector<std::shared_ptr<Station>> list;
    for (const auto& st : stations_) {
        if (st->getStationType() == type) {
            list.push_back(st);
        }
    }
    return list;
}

std::shared_ptr<Station> Facility::findFreeStation(StationType type) const {
    for (const auto& st : stations_) {
        if (st->getStationType() == type && st->isFree()) {
            return st;
        }
    }
    return nullptr;
}

int Facility::getTotalStationCount(StationType type) const {
    int count = 0;
    for (const auto& st : stations_) {
        if (st->getStationType() == type) {
            count++;
        }
    }
    return count;
}

int Facility::getFreeStationCount(StationType type) const {
    int count = 0;
    for (const auto& st : stations_) {
        if (st->getStationType() == type && st->isFree()) {
            count++;
        }
    }
    return count;
}

int Facility::getBusyStationCount(StationType type) const {
    int count = 0;
    for (const auto& st : stations_) {
        if (st->getStationType() == type && st->isBusy()) {
            count++;
        }
    }
    return count;
}

void Facility::addRestaurantId(int restaurantId) {
    restaurantIds_.insert(restaurantId);
}

bool Facility::hasRestaurantId(int restaurantId) const {
    return restaurantIds_.find(restaurantId) != restaurantIds_.end();
}

const std::set<int>& Facility::getRestaurantIds() const {
    return restaurantIds_;
}

} // namespace QuickServe
