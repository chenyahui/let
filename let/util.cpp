//
// Created by yahuichen on 2019/1/23.
//

#include "util.h"

namespace let {
    std::string now_time_str() {
        time_t rawtime;
        struct tm *timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return {buffer};
    }
}