#pragma once
#include "core/Types.h"
#include <vector>
namespace belyaev::indicators {
class CCI {
public:
    static std::vector<double> calculate(const std::vector<core::Bar>& bars, int period);
private:
    static double typical_price(const core::Bar& bar);
};
} // namespace belyaev::indicators
