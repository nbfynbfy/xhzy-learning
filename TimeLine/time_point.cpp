//
// Created by nbfynbfy on 2020/8/21.
//

#include "time_point.h"
#include "chrono"

TimePoint::TimePoint() {
    auto time = std::chrono::steady_clock::now().time_since_epoch();
    time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(time).count();
}

TimePeriod TimePoint::operator-(TimePoint other) const {
    return TimePeriod(time_-other.time_);
}

TimePoint TimePoint::operator+(TimePeriod time_period) const {
    return TimePoint(time_+time_period.GetPeriod());
}

TimePoint TimePoint::operator-(TimePeriod time_period) const {
    return TimePoint(time_-time_period.GetPeriod());
}