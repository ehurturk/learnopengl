#include "Utils.hpp"

float clamp(float val, float min, float max) {
    return MAX(min, MIN(val, max));
}