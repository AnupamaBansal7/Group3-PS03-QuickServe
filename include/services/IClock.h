#ifndef QUICKSERVE_SERVICES_ICLOCK_H
#define QUICKSERVE_SERVICES_ICLOCK_H

#include <chrono>

namespace QuickServe {

class IClock {
public:
    virtual ~IClock() = default;
    virtual std::chrono::system_clock::time_point now() const = 0;
    virtual void advanceMinutes(double /*minutes*/) {}
};

class SystemClock : public IClock {
public:
    std::chrono::system_clock::time_point now() const override {
        return std::chrono::system_clock::now();
    }
};

class ControllableClock : public IClock {
public:
    explicit ControllableClock(std::chrono::system_clock::time_point initialTime = std::chrono::system_clock::now())
        : currentTime_(initialTime) {}

    std::chrono::system_clock::time_point now() const override {
        return currentTime_;
    }

    void setTime(std::chrono::system_clock::time_point t) {
        currentTime_ = t;
    }

    void advanceMinutes(double minutes) override {
        auto durMs = std::chrono::milliseconds(static_cast<long long>(minutes * 60000.0));
        currentTime_ += durMs;
    }

    void advanceSeconds(int seconds) {
        currentTime_ += std::chrono::seconds(seconds);
    }

private:
    std::chrono::system_clock::time_point currentTime_;
};

} // namespace QuickServe

#endif // QUICKSERVE_SERVICES_ICLOCK_H
