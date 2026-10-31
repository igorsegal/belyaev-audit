#pragma once
#include "core/Types.h"
#include <vector>
#include <string>
namespace belyaev::indicators {
class SixLinesIndicator {
public:
    struct Result {
        int direction = 0;
        int points = 0;
        std::string description;
    };
    static Result analyze(
        const std::vector<double>& stoch_k,
        const std::vector<double>& stoch_d,
        const std::vector<double>& cci_fast,
        const std::vector<double>& cci_slow,
        const std::vector<double>& rsi_fast,
        const std::vector<double>& rsi_slow
    );
};
}
