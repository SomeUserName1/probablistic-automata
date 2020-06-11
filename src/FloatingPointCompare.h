#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_FLOATINGPOINTCOMPARE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_FLOATINGPOINTCOMPARE_H

#include <concepts>
#include <limits>
#include <algorithm>

template <typename T>
requires (std::is_floating_point_v<T>)
static bool floating_point_compare(T x, T y) {
    return std::fabs(x - y) <= std::numeric_limits<T>::epsilon() * std::max({1.0, std::fabs(x), std::fabs(y)});
}

#endif //STOCHASTIC_SYSTEM_MINIMIZATION_FLOATINGPOINTCOMPARE_H
