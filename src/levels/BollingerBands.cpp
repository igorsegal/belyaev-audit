#include "levels/BollingerBands.h"
#include <cmath>
namespace belyaev::levels {
BollingerBands::Result BollingerBands::calculate(const std::vector<core::Bar>& bars, int period, double width) {
    Result res;
    res.upper.assign(bars.size(), 0.0);
    res.middle.assign(bars.size(), 0.0);
    res.lower.assign(bars.size(), 0.0);
    if (bars.size() < static_cast<size_t>(period)) return res;
    for (size_t i = period - 1; i < bars.size(); ++i) {
        double sum = 0.0;
        for (size_t j = i - period + 1; j <= i; ++j) {
            sum += bars[j].close;
        }
        double sma = sum / period;
        res.middle[i] = sma;
        double variance = 0.0;
        for (size_t j = i - period + 1; j <= i; ++j) {
            variance += std::pow(bars[j].close - sma, 2);
        }
        double stddev = std::sqrt(variance / period);
        res.upper[i] = sma + (width * stddev);
        res.lower[i] = sma - (width * stddev);
    }
    return res;
}
}
