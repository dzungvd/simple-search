#ifndef UTILS_H
#define UTILS_H

#include <string>

extern "C" {
#include "sodium.h"
}
class Utils
{
public:
    Utils();
    static std::string convertToBase64 (const unsigned char* input, size_t input_len);
    static std::string convertFromB64ToBin (const char* input, unsigned long long input_len);
};

#endif // UTILS_H
