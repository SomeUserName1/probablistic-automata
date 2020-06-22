#ifndef STOCHASTIC_SYSTEM_MINIMIZATION_FLOATINGPOINTCOMPARE_H
#define STOCHASTIC_SYSTEM_MINIMIZATION_FLOATINGPOINTCOMPARE_H

#include <algorithm>
#include <concepts>
#include <limits>

template <typename T>
requires(std::is_floating_point_v<T>) static auto floating_point_compare(T x,
                                                                         T y) -> bool {
  return std::fabs(x - y) <= 1e-6 *
                                 std::max({1.0, std::fabs(x), std::fabs(y)});
}

#endif // STOCHASTIC_SYSTEM_MINIMIZATION_FLOATINGPOINTCOMPARE_H
