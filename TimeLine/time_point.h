//
// Created by nbfynbfy on 2020/8/21.
//

#ifndef TIMELINE_TIME_POINT_H
#define TIMELINE_TIME_POINT_H

#include <cstdint>
#include <chrono>
#include "time_period.h"

class TimePoint {
public:
    TimePoint();
    //  直接构造一个当前时间的TimePoint
    TimePoint(int64_t time) { time_ = time; }
    //  根据给定时间构造一个TimePoint
    ~TimePoint() {}

    static TimePoint Now() { return TimePoint(); }

    void SetTime(int64_t time) { time_ = time; }
    int64_t GetTime() { return time_; }

    TimePeriod operator-(TimePoint other) const;
    TimePoint operator+(TimePeriod time_period) const;
    TimePoint operator-(TimePeriod time_period) const;

    bool operator==(TimePoint other) const { return time_ == other.time_; }
    bool operator!=(TimePoint other) const { return time_ != other.time_; }
    bool operator<(TimePoint other) const { return time_ < other.time_; }
    bool operator<=(TimePoint other) const { return time_ <= other.time_; }
    bool operator>(TimePoint other) const { return time_ > other.time_; }
    bool operator>=(TimePoint other) const { return time_ >= other.time_; }

private:
    int64_t time_;
};

#endif //TIMELINE_TIME_POINT_H
