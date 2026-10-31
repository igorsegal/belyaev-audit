#include "indicators/Stochastic.h"
#include <algorithm>
#include <stdexcept>
namespace belyaev::indicators {
void Stochastic::calculate(const std::vector<core::Bar>& bars, int k_period, int d_period, 
                           std::vector<double>& out_k, std::vector<double>& out_d) {
    if (k_period <= 0 || d_period <= 0) throw std::invalid_argument("Stoch periods must be > 0");
    out_k.assign(bars.size(), 0.0);
    out_d.assign(bars.size(), 0.0);
    if (bars.size() < static_cast<size_t>(k_period)) return;
    for (size_t i = k_period - 1; i < bars.size(); ++i) {
        double highest_high = bars[i].high;
        double lowest_low = bars[i].low;
        for (size_t j = i - k_period + 1; j <= i; ++j) {
            highest_high = std::max(highest_high, bars[j].high);
            lowest_low = std::min(lowest_low, bars[j].low);
        }
        double range = highest_high - lowest_low;
        out_k[i] = (range == 0.0) ? 0.0 : 100.0 * (bars[i].close - lowest_low) / range;
    }
    double sum_d = 0.0;
    for (size_t i = k_period - 1; i < bars.size(); ++i) {
        sum_d += out_k[i];
        if (i >= static_cast<size_t>(k_period - 1 + d_period - 1)) {
            out_d[i] = sum_d / d_period;
            sum_d -= out_k[i - d_period + 1];
        }
    }
}
}
