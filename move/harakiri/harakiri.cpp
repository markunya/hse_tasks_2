#include "harakiri.h"

std::string AwesomeCallback::operator()() const&& {
    std::string t = std::move(str_);
    delete this;
    return t;
}
