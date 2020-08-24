//
// Created by nbfynbfy on 2020/8/21.
//

#ifndef TIMELINE_CLOCK_DARWIN_H
#define TIMELINE_CLOCK_DARWIN_H

#include "time_point.h"
#include "CoreFoundation/CFRunLoop.h"
#include "wakeable.h"

static constexpr CFTimeInterval kDistantFuture = 1e10;

class ClockDarwin: public Wakeable {
public:
    ClockDarwin();
    ~ClockDarwin();

    void Run();
    void Terminate();
    void WakeUp(TimePoint time_point);
    static void OnTimerFire(CFRunLoopTimerRef timer, ClockDarwin* loop);
    virtual void TimerFire() = 0;
    virtual void TimerEnd() = 0;

private:
    CFRunLoopTimerRef delayed_wake_timer_;
    CFRunLoopRef loop_;
    bool running_;
};

#endif //TIMELINE_CLOCK_DARWIN_H
