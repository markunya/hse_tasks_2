#pragma once

#include "object.h"
#include "tokenizer.h"
#include <memory>
#include <cstring>

std::shared_ptr<Object> ReadList(Tokenizer*);

std::shared_ptr<Object> ReadDefault(Tokenizer*);

std::shared_ptr<Object> Read(Tokenizer*);

