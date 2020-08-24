cmake_minimum_required(VERSION 3.13)
project(TimeLine)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_EXE_LINKER_FLAGS "-framework IOKit -framework Security -framework CFNetwork -framework CoreText -framework OpenGL -framework CoreServices -framework CoreFoundation -framework CoreGraphics -framework AudioToolbox -framework CoreVideo -framework VideoToolbox -framework CoreMedia -framework CoreImage -framework Cocoa")

add_executable(TimeLine main.cpp time_point.cpp time_point.h time_period.cpp time_period.h clock_darwin.h clock_darwin.mm wakeable.h time_line.cpp time_line.h)
