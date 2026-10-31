#include "indicators/ROC.h"
#include <stdexcept>
namespace belyaev::indicators {
std::vector<double> ROC::calculate(const std::vector<core::Bar>& bars, int period) {
    if (period <= 0) throw std::invalid_argument("ROC period must be > 0");
    std::vector<double> roc(bars.size(), 0.0);
    if (bars.size() <= static_cast<size_t>(period)) return roc;
    for (size_t i = period; i < bars.size(); ++i) {
        if (bars[i - period].close != 0.0) {
            roc[i] = 100.0 * (bars[i].close - bars[i - period].close) / bars[i - period].close;
        }
    }
    return roc;
}
}
