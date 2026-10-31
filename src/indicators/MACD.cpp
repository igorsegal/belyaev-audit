#include "indicators/MACD.h"
#include "indicators/EMA.h"
namespace belyaev::indicators {
MACD::Result MACD::calculate(const std::vector<core::Bar>& bars) {
    MACD::Result res;
    res.macd_line.assign(bars.size(), 0.0);
    res.histogram.assign(bars.size(), 0.0);
    if (bars.empty()) return res;
    auto ema8 = EMA::calculate(bars, 8);
    auto ema13 = EMA::calculate(bars, 13);
    // MACD(L) = EMA(8) - EMA(13)
    std::vector<double> diff(bars.size(), 0.0);
    for (size_t i = 0; i < bars.size(); ++i) {
        if (ema8[i] != 0.0 && ema13[i] != 0.0) {
            diff[i] = ema8[i] - ema13[i];
            res.macd_line[i] = diff[i];
        }
    }
    // MACD(G) = EMA(21) от MACD(L)
    // Сглаживаем только валидные значения
    std::vector<double> valid_diff;
    std::vector<size_t> valid_indices;
    for (size_t i = 0; i < bars.size(); ++i) {
        if (ema8[i] != 0.0 && ema13[i] != 0.0) {
            valid_diff.push_back(diff[i]);
            valid_indices.push_back(i);
        }
    }
    if (valid_diff.size() > 21) {
        std::vector<double> ema21(valid_diff.size(), 0.0);
        double seed = 0.0;
        for (int i = 0; i < 21; ++i) seed += valid_diff[i];
        seed /= 21.0;
        ema21[20] = seed;
        const double k = 2.0 / (21.0 + 1.0);
        for (size_t i = 21; i < valid_diff.size(); ++i) {
            ema21[i] = (valid_diff[i] - ema21[i - 1]) * k + ema21[i - 1];
        }
        for (size_t i = 20; i < valid_diff.size(); ++i) {
            res.histogram[valid_indices[i]] = ema21[i];
        }
    }
    return res;
}
}
