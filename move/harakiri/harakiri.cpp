#include "harakiri.h"

std::string AwesomeCallback::operator()() const&& {
    return std::string(str_);
}


