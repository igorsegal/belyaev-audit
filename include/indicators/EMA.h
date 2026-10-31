#pragma once
#include "core/Types.h"
#include <vector>
namespace belyaev::indicators {
class EMA {
public:
    static std::vector<double> calculate(const std::vector<core::Bar>& bars, int period);
private:
    static double init_seed(const std::vector<core::Bar>& bars, int period);
};
} // namespace belyaev::indicators
