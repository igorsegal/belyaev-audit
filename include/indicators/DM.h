#pragma once
#include "core/Types.h"
#include <vector>
namespace belyaev::indicators {
class DM {
public:
    struct Result {
        std::vector<double> plus_dm;   // +DM
        std::vector<double> minus_dm;  // -DM
        std::vector<double> adx;       // ADX
    };
    static Result calculate(const std::vector<core::Bar>& bars, int period = 8);
};
}
