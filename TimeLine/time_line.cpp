//
// Created by nbfynbfy on 2020/8/24.
//

#include <iostream>
#include "time_line.h"

void TimeLine::ClearTimeLine() {
    while (!timepacket_queue_.empty()) timepacket_queue_.pop();
}

void TimeLine::AddTimePacket(TimePacket time_packet) {
    if (time_packet.pts_ > TimePoint::Now()) {
        if (timepacket_queue_.empty() || time_packet.pts_ < timepacket_queue_.top().pts_) {
            WakeUp(time_packet.pts_);
        }
        timepacket_queue_.push(time_packet);
    }
}

void TimeLine::TimerFire() {
    if (timepacket_queue_.empty()) {
        std::cout << "TimePacket queue is empty." << std::endl;
        return;
    }

    TimePacket time_packet = timepacket_queue_.top();
    timepacket_queue_.pop();

    if (time_packet.func_ != nullptr) {
        time_packet.func_(time_packet.data_);
    }

    if (!timepacket_queue_.empty()) {
        WakeUp(timepacket_queue_.top().pts_);
    }
}

void TimeLine::TimerEnd() {
    std::cout << "TimeLine is end." << std::endl;
}