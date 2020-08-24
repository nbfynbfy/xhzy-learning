#include <iostream>
#include "time_line.h"

void func(void* data) {
    std::cout << "data: " << *(int*)data << std::endl;
}

int main() {
    int *data1 = new int(1);
    int *data2 = new int(2);
    int *data3 = new int(3);
    TimePacket tp1(reinterpret_cast<void *>(data1), func, TimePoint::Now() + TimePeriod(1e9));
    TimePacket tp2(reinterpret_cast<void *>(data2), func, TimePoint::Now() + TimePeriod(2e9));
    TimePacket tp3(reinterpret_cast<void *>(data3), func, TimePoint::Now() + TimePeriod(3e9));

    TimeLine time_line;
    time_line.AddTimePacket(tp3);
    time_line.AddTimePacket(tp2);
    time_line.AddTimePacket(tp1);

    std::cout<<"begin:"<<std::endl;

    time_line.Run();
}
