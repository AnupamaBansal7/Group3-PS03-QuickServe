#include "repositories/FacilityRepository.h"

namespace QuickServe {

void FacilityRepository::add(std::shared_ptr<Facility> facility) {
    if (facility) {
        if (facilities_.empty() || activeFacilityId_.empty()) {
            activeFacilityId_ = facility->getFacilityId();
        }
        facilities_[facility->getFacilityId()] = facility;
    }
}

std::shared_ptr<Facility> FacilityRepository::findById(const std::string& facilityId) const {
    auto it = facilities_.find(facilityId);
    if (it != facilities_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Facility>> FacilityRepository::getAll() const {
    std::vector<std::shared_ptr<Facility>> list;
    for (const auto& pair : facilities_) {
        list.push_back(pair.second);
    }
    return list;
}

std::shared_ptr<Facility> FacilityRepository::getActiveFacility() const {
    return findById(activeFacilityId_);
}

void FacilityRepository::setActiveFacilityId(const std::string& facilityId) {
    activeFacilityId_ = facilityId;
}

} // namespace QuickServe
