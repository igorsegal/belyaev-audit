#include "indicators/RSI.h"
#include <stdexcept>
namespace belyaev::indicators {
std::vector<double> RSI::calculate(const std::vector<core::Bar>& bars, int period) {
    if (period <= 0) throw std::invalid_argument("RSI period must be > 0");
    std::vector<double> rsi(bars.size(), 0.0);
    if (bars.size() <= static_cast<size_t>(period)) return rsi;
    double avg_gain = 0.0, avg_loss = 0.0;
    for (int i = 1; i <= period; ++i) {
        double change = bars[i].close - bars[i - 1].close;
        if (change > 0) avg_gain += change;
        else avg_loss -= change;
    }
    avg_gain /= period;
    avg_loss /= period;
    rsi[period] = (avg_loss == 0.0) ? 100.0 : 100.0 - (100.0 / (1.0 + (avg_gain / avg_loss)));
    for (size_t i = period + 1; i < bars.size(); ++i) {
        double change = bars[i].close - bars[i - 1].close;
        double gain = (change > 0) ? change : 0.0;
        double loss = (change < 0) ? -change : 0.0;
        avg_gain = ((avg_gain * (period - 1)) + gain) / period;
        avg_loss = ((avg_loss * (period - 1)) + loss) / period;
        rsi[i] = (avg_loss == 0.0) ? 100.0 : 100.0 - (100.0 / (1.0 + (avg_gain / avg_loss)));
    }
    return rsi;
}
}
