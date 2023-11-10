#pragma once

#include <string>
#include "object.h"

class Interpreter {
public:
    std::string Run(const std::string&);

    std::shared_ptr<Object> BuildInAst(const std::string&);
    std::shared_ptr<Object> BuildOutAst(std::shared_ptr<Object>);
};
