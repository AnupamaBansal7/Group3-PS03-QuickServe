#ifndef QUICKSERVE_REPOSITORIES_FACILITYREPOSITORY_H
#define QUICKSERVE_REPOSITORIES_FACILITYREPOSITORY_H

#include "models/Facility.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace QuickServe {

class FacilityRepository {
public:
    void add(std::shared_ptr<Facility> facility);
    std::shared_ptr<Facility> findById(const std::string& facilityId) const;
    std::vector<std::shared_ptr<Facility>> getAll() const;

    std::shared_ptr<Facility> getActiveFacility() const;
    void setActiveFacilityId(const std::string& facilityId);

private:
    std::map<std::string, std::shared_ptr<Facility>> facilities_;
    std::string activeFacilityId_;
};

} // namespace QuickServe

#endif // QUICKSERVE_REPOSITORIES_FACILITYREPOSITORY_H
