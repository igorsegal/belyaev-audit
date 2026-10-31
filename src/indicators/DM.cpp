#include "indicators/DM.h"
#include <cmath>
#include <algorithm>
namespace belyaev::indicators {
DM::Result DM::calculate(const std::vector<core::Bar>& bars, int period) {
    DM::Result res;
    size_t n = bars.size();
    res.plus_dm.assign(n, 0.0);
    res.minus_dm.assign(n, 0.0);
    res.adx.assign(n, 0.0);
    if (n < 2) return res;
    std::vector<double> tr(n, 0.0);
    std::vector<double> pdm(n, 0.0);
    std::vector<double> mdm(n, 0.0);
    for (size_t i = 1; i < n; ++i) {
        double up_move = bars[i].high - bars[i-1].high;
        double down_move = bars[i-1].low - bars[i].low;
        if (up_move > down_move && up_move > 0) pdm[i] = up_move;
        else pdm[i] = 0.0;
        if (down_move > up_move && down_move > 0) mdm[i] = down_move;
        else mdm[i] = 0.0;
        double hl = bars[i].high - bars[i].low;
        double hc = std::abs(bars[i].high - bars[i-1].close);
        double lc = std::abs(bars[i].low - bars[i-1].close);
        tr[i] = std::max({hl, hc, lc});
    }
    // Сглаживание (Wilder)
    double smooth_tr = 0.0, smooth_pdm = 0.0, smooth_mdm = 0.0;
    for (int i = 1; i <= period; ++i) {
        smooth_tr += tr[i];
        smooth_pdm += pdm[i];
        smooth_mdm += mdm[i];
    }
    std::vector<double> dx(n, 0.0);
    for (size_t i = period; i < n; ++i) {
        if (i > static_cast<size_t>(period)) {
            smooth_tr = smooth_tr - (smooth_tr / period) + tr[i];
            smooth_pdm = smooth_pdm - (smooth_pdm / period) + pdm[i];
            smooth_mdm = smooth_mdm - (smooth_mdm / period) + mdm[i];
        }
        double di_plus = (smooth_tr == 0.0) ? 0.0 : 100.0 * smooth_pdm / smooth_tr;
        double di_minus = (smooth_tr == 0.0) ? 0.0 : 100.0 * smooth_mdm / smooth_tr;
        res.plus_dm[i] = di_plus;
        res.minus_dm[i] = di_minus;
        double di_sum = di_plus + di_minus;
        dx[i] = (di_sum == 0.0) ? 0.0 : 100.0 * std::abs(di_plus - di_minus) / di_sum;
    }
    // ADX - сглаженный DX
    double adx_val = 0.0;
    for (int i = period; i < period * 2; ++i) {
        adx_val += dx[i];
    }
    adx_val /= period;
    res.adx[period * 2 - 1] = adx_val;
    for (size_t i = period * 2; i < n; ++i) {
        adx_val = ((adx_val * (period - 1)) + dx[i]) / period;
        res.adx[i] = adx_val;
    }
    return res;
}
}
