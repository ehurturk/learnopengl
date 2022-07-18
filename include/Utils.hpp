#ifndef UTILS_HPP_
#define UTILS_HPP_

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

float clamp(float val, float min, float max);

#endif