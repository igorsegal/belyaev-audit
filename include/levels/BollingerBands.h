#pragma once
#include "core/Types.h"
#include <vector>
namespace belyaev::levels {
class BollingerBands {
public:
    struct Result {
        std::vector<double> upper;
        std::vector<double> middle;
        std::vector<double> lower;
    };
    static Result calculate(const std::vector<core::Bar>& bars, int period = 21, double width = 2.0);
};
}
