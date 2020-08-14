#ifndef NEL_LOG_HH
#define NEL_LOG_HH

#include <cstdio>

namespace nel {

class Log {
    public:
        friend Log &operator<<(Log &outs, char const *v) {
            fprintf(stderr, "%s", v);
            return outs;
        }
        friend Log &operator<<(Log &outs, size_t const v) {
            fprintf(stderr, "%zu", v);
            return outs;
        }
};

extern Log log;

}

#endif//NEL_LOG_HH
