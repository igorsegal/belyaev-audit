#include "rules/RunAnalyzer.h"
#include <cmath>
namespace belyaev::rules {
RunResult RunAnalyzer::analyze(
    const std::vector<double>& ind, 
    const std::vector<core::Bar>& bars, 
    int heavy_chart_trend
) {
    RunResult res;
    res.magnitude = 0.0;
    res.direction = RunDirection::Flat;
    res.is_valid = true;
    if (ind.size() < 2 || bars.size() < 2) return res;
    size_t curr_idx = ind.size() - 1;
    size_t prev_idx = ind.size() - 2;
    res.magnitude = ind[curr_idx] - ind[prev_idx];
    if (res.magnitude > 0.0) res.direction = RunDirection::Up;
    else if (res.magnitude < 0.0) res.direction = RunDirection::Down;
    bool is_white_candle = bars[curr_idx].close > bars[curr_idx].open;
    bool is_black_candle = bars[curr_idx].close < bars[curr_idx].open;
    if (res.direction == RunDirection::Up && is_black_candle) {
        res.is_valid = false;
        res.reason = "Быстрый прогон вверх на черной свече";
    }
    else if (res.direction == RunDirection::Down && is_white_candle) {
        res.is_valid = false;
        res.reason = "Быстрый прогон вниз на белой свече";
    }
    if (heavy_chart_trend != 0) {
        if (heavy_chart_trend == 1 && res.direction == RunDirection::Down) {
            res.reason += " | Прогон против тяжелого графика";
        }
    }
    return res;
}
} // namespace belyaev::rules
