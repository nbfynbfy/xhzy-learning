//
// Created by nbfynbfy on 2020/8/24.
//

#ifndef TIMELINE_TIME_LINE_H
#define TIMELINE_TIME_LINE_H

#if OS_LINUX
#else OS_MACOSX
#include "clock_darwin.h"
#endif

#include "queue"
#include "time_point.h"

struct TimePacket {
    void* data_;
    void (*func_)(void* data);
    TimePoint pts_;
    TimePacket(void* data, void (*func)(void* data), TimePoint pts) {
        data_ = data;
        func_ = func;
        pts_ = pts;
    }   //  对data做func函数运算
    ~TimePacket() {}

    bool operator>(TimePacket other) const { return pts_>other.pts_; }
    bool operator>=(TimePacket other) const { return pts_>=other.pts_; }
    bool operator==(TimePacket other) const { return pts_==other.pts_; }
    bool operator!=(TimePacket other) const { return pts_!=other.pts_; }
    bool operator<=(TimePacket other) const { return pts_<=other.pts_; }
    bool operator<(TimePacket other) const { return pts_<other.pts_; }
};

class TimeLine
#ifdef
    :public ClockDarwin
#elif TARGET_OS_WIN32
#elif TARGET_OS_UNIX
#endif
        {
public:
    TimeLine() { ClearTimeLine(); }
    ~TimeLine() { ClearTimeLine(); }

    bool HasTimePacket() { return timepacket_queue_.empty(); }
    void ClearTimeLine();
    void AddTimePacket(TimePacket time_packet);

    void TimerFire();
    void TimerEnd();

private:
    std::priority_queue<TimePacket, std::vector<TimePacket>, std::greater<TimePacket> > timepacket_queue_;  //  TimePacket的优先队列
};

#endif //TIMELINE_TIME_LINE_H
