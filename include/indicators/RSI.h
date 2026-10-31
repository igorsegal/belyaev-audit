#pragma once
#include "core/Types.h"
#include <vector>
namespace belyaev::indicators {
class RSI {
public:
    static std::vector<double> calculate(const std::vector<core::Bar>& bars, int period);
};
}
