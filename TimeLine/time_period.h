//
// Created by nbfynbfy on 2020/8/21.
//

#ifndef TIMELINE_TIME_PERIOD_H
#define TIMELINE_TIME_PERIOD_H

#include <cstdint>

class TimePeriod {
public:
    TimePeriod() { period_ = 0; }
    TimePeriod(int64_t period) { period_ = period; }
    ~TimePeriod() {}

    static TimePeriod FromNanoSecond(int64_t period) { return TimePeriod(period); }
    static TimePeriod FromMicroSecond(int64_t period) { return FromNanoSecond(period*1000); }
    static TimePeriod FromMilliSecond(int64_t period) { return FromMicroSecond(period*1000); }
    static TimePeriod FromSecond(int64_t period) { return FromMilliSecond(period*1000); }

    int64_t  ToNanoSecond() { return period_; }
    int64_t  ToMicroSecond() { return ToNanoSecond()/1000; }
    int64_t  ToMilliSecond() { return ToMicroSecond()/1000; }
    int64_t  ToSecond() { return ToMilliSecond()/1000; }

    double ToNanoSecondF() { return period_; }
    double ToMicroSecondF() { return ToNanoSecondF()/1000.0; }
    double ToMilliSecondF() { return ToMicroSecondF()/1000.0; }
    double ToSecondF() { return ToMilliSecondF()/1000.0; }

    void SetPeriod(int64_t period) { period_ = period; }
    int64_t GetPeriod() { return period_; }

    TimePeriod operator-(TimePeriod other) const { return FromNanoSecond(period_ - other.period_); }
    TimePeriod operator+(TimePeriod other) const { return FromNanoSecond(period_ + other.period_); }
    TimePeriod operator/(int64_t divisor) const { return FromNanoSecond(period_ / divisor); }
    int64_t operator/(TimePeriod other) const { return period_ / other.period_; }
    TimePeriod operator*(int64_t multiplier) const { return FromNanoSecond(period_ * multiplier); }
    TimePeriod operator%(TimePeriod other) const { return FromNanoSecond(period_ % other.period_); }

    bool operator==(TimePeriod other) const { return period_ == other.period_; }
    bool operator!=(TimePeriod other) const { return period_ != other.period_; }
    bool operator<(TimePeriod other) const { return period_ < other.period_; }
    bool operator<=(TimePeriod other) const { return period_ <= other.period_; }
    bool operator>(TimePeriod other) const { return period_ > other.period_; }
    bool operator>=(TimePeriod other) const { return period_ >= other.period_; }

private:
    int64_t period_;
};
#endif //TIMELINE_TIME_PERIOD_H
