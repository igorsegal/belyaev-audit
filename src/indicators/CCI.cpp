#include "indicators/CCI.h"
#include <cmath>
#include <stdexcept>
namespace belyaev::indicators {
double CCI::typical_price(const core::Bar& bar) {
    return (bar.high + bar.low + bar.close) / 3.0;
}
std::vector<double> CCI::calculate(const std::vector<core::Bar>& bars, int period) {
    if (period <= 0) throw std::invalid_argument("CCI period must be > 0");
    std::vector<double> cci(bars.size(), 0.0);
    if (bars.size() < static_cast<size_t>(period)) return cci;
    std::vector<double> tp(bars.size());
    for (size_t i = 0; i < bars.size(); ++i) {
        tp[i] = typical_price(bars[i]);
    }
    double sum_tp = 0.0;
    for (int i = 0; i < period; ++i) {
        sum_tp += tp[i];
    }
    double sma = sum_tp / period;
    for (size_t i = period - 1; i < bars.size(); ++i) {
        if (i >= static_cast<size_t>(period)) {
            sum_tp = sum_tp - tp[i - period] + tp[i];
            sma = sum_tp / period;
        }
        double mean_dev = 0.0;
        for (size_t j = i - period + 1; j <= i; ++j) {
            mean_dev += std::abs(tp[j] - sma);
        }
        mean_dev /= period;
        if (mean_dev == 0.0) {
            cci[i] = 0.0;
        } else {
            cci[i] = (tp[i] - sma) / (0.015 * mean_dev);
        }
    }
    return cci;
}
} // namespace belyaev::indicators
