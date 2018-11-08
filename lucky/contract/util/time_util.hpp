#pragma once

class time_util {
public:
    static uint32_t origin;
    static uint32_t day;
    static uint32_t hour;
    static uint32_t min;

public:
    static uint32_t getnow() {
        return now() - origin;
    }

    static uint32_t nextday() {
        uint32_t t1 = now();
        uint32_t t2 = t1 - (t1 + 8 * hour) % day + day;
        return t2 - origin;
    }
};

uint32_t time_util::origin = 1540000000;
uint32_t time_util::day = 86400;
uint32_t time_util::hour = 3600;
uint32_t time_util::min = 60;
