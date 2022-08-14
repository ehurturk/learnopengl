#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <array>
#include <iostream>
#include <stdint.h>
#include <unordered_map>
#include <map>

using ui8  = uint8_t;
using ui16 = uint16_t;
using ui32 = uint32_t;
using ui64 = uint64_t;

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

#define ARR_SIZE(x) sizeof(x) / sizeof(x[0])
