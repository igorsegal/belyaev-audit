#pragma once
#include "core/Types.h"
#include <vector>
namespace belyaev::indicators {
class MACD {
public:
    struct Result {
        std::vector<double> macd_line;    // MACD(L) = EMA(8) - EMA(13)
        std::vector<double> histogram;    // MACD(G) = EMA(21) от MACD(L)
    };
    static Result calculate(const std::vector<core::Bar>& bars);
};
}
