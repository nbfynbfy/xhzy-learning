//
// Created by nbfynbfy on 2020/8/21.
//

#include <iostream>
#include <cmath>
#include "clock_darwin.h"

ClockDarwin::ClockDarwin() {
    running_ = false;
    loop_ = (CFRunLoopRef) CFRetain(CFRunLoopGetCurrent());
    // Setup the delayed wake source.
    CFRunLoopTimerContext timer_context = {
            .info = this,
    };
    delayed_wake_timer_ = CFRunLoopTimerCreate(kCFAllocatorDefault, kDistantFuture /* fire date */,
                                 HUGE_VAL /* interval */, 0 /* flags */, 0 /* order */,
                                 reinterpret_cast<CFRunLoopTimerCallBack>(&ClockDarwin::OnTimerFire) /* callout */,
                                 &timer_context /* context */);
    CFRunLoopAddTimer(loop_, delayed_wake_timer_, kCFRunLoopCommonModes);
}

ClockDarwin::~ClockDarwin() {
    CFRunLoopTimerInvalidate(delayed_wake_timer_);
    CFRunLoopRemoveTimer(loop_, delayed_wake_timer_, kCFRunLoopCommonModes);
}

void ClockDarwin::Run() {
    running_ = true;
    while (running_) {
        @autoreleasepool {
                int result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, kDistantFuture, TRUE);
                if (result == kCFRunLoopRunStopped || result == kCFRunLoopRunFinished) {
//                    This handles the case where the loop is terminated using
//                    CoreFoundation APIs.
                    @autoreleasepool {
                            TimerEnd();
                    }
                    running_ = false;
                }
        }
    }
}

void ClockDarwin::Terminate() {
    running_ = false;
    CFRunLoopStop(loop_);
}

void ClockDarwin::WakeUp(TimePoint time_point) {
    CFRunLoopTimerSetNextFireDate(
            delayed_wake_timer_,
            CFAbsoluteTimeGetCurrent() + (time_point - TimePoint::Now()).ToSecondF());
}

void ClockDarwin::OnTimerFire(CFRunLoopTimerRef timer, ClockDarwin* loop) {
    @autoreleasepool{
            loop->TimerFire();
    }
}
