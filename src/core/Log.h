#ifndef TINY_COMPILE_LOG_H
#define TINY_COMPILE_LOG_H

#include <iostream>
#include <string>

class Log {
    static std::ostream* out;
    static int error_count;
public:
    static inline void setOutputStream(std::ostream* output_stream) {
        out = output_stream;
    }
    static inline int getErrorCount() {
        return error_count;
    }
    static inline bool hasError() {
        return error_count != 0;
    }

    static inline void error(const std::string &info, int line, int offset) {
        if (out)
            *out << "ERROR IN LINE " << line << ":" << offset << " " << info << std::endl;
        error_count ++;
    }

    static inline void parse_error(const std::string &info, int line, int offset) {
        if (out)
            *out << "PARSE ERROR IN LINE " << line << ":" << offset << " " << info << std::endl;
        error_count ++;
    }
};

#endif //TINY_COMPILE_LOG_H
