#include "indicators/EMA.h"
#include <stdexcept>
namespace belyaev::indicators {
std::vector<double> EMA::calculate(const std::vector<core::Bar>& bars, int period) {
    if (period <= 0) throw std::invalid_argument("EMA period must be > 0");
    std::vector<double> ema(bars.size(), 0.0);
    if (bars.size() < static_cast<size_t>(period)) return ema;
    double seed = init_seed(bars, period);
    ema[period - 1] = seed;
    const double k = 2.0 / (period + 1.0);
    for (size_t i = period; i < bars.size(); ++i) {
        ema[i] = (bars[i].close - ema[i - 1]) * k + ema[i - 1];
    }
    return ema;
}
double EMA::init_seed(const std::vector<core::Bar>& bars, int period) {
    double sum = 0.0;
    for (int i = 0; i < period; ++i) {
        sum += bars[i].close;
    }
    return sum / period;
}
} // namespace belyaev::indicators
