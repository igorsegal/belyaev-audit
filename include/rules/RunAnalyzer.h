#pragma once
#include "core/Types.h"
#include <vector>
#include <string>
namespace belyaev::rules {
enum class RunDirection {
    Up,
    Down,
    Flat
};
struct RunResult {
    RunDirection direction;
    double magnitude;
    bool is_valid;
    std::string reason;
};
class RunAnalyzer {
public:
    static RunResult analyze(
        const std::vector<double>& indicator_values,
        const std::vector<core::Bar>& bars,
        int heavy_chart_trend
    );
};
} // namespace belyaev::rules
