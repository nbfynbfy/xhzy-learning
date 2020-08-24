//
// Created by nbfynbfy on 2020/8/21.
//

#ifndef TIMELINE_WAKEABLE_H
#define TIMELINE_WAKEABLE_H

#include "time_point.h"

class Wakeable {
public:
    virtual ~Wakeable() {}

    virtual void WakeUp(TimePoint time_point) = 0;
};

#endif //TIMELINE_WAKEABLE_H
