#include "rules/DivergenceFinder.h"
#include <algorithm>
#include <cmath>
#include <climits>
namespace belyaev::rules {
DivergenceFinder::Result DivergenceFinder::find(
    const std::vector<double>& ind,
    const std::vector<core::Bar>& bars,
    int lookback,
    int fractal_k
) {
    Result res;
    if (lookback <= 0 || fractal_k <= 0) return res;
    if (ind.size() < 10 || bars.size() < 10) return res;
    size_t n = std::min(ind.size(), bars.size());
    size_t fk = static_cast<size_t>(fractal_k);
    if (n <= 2 * fk + 1) return res;
    size_t start = (n > static_cast<size_t>(lookback)) ? n - static_cast<size_t>(lookback) : 0;
    if (start < fk + 1) start = fk + 1;
    size_t last_visible = n - 1 - fk;
    // ФИКС: если start > last_visible — недостаточно данных для двух фракталов
    if (start > last_visible) return res;
    // ============ 1. Фракталы цены ============
    std::vector<size_t> price_highs, price_lows;
    for (size_t i = start; i <= last_visible; ++i) {
        bool is_high = true, is_low = true;
        for (size_t k = 1; k <= fk; ++k) {
            if (bars[i].high <= bars[i - k].high || bars[i].high <= bars[i + k].high) is_high = false;
            if (bars[i].low  >= bars[i - k].low  || bars[i].low  >= bars[i + k].low)  is_low  = false;
        }
        if (is_high) price_highs.push_back(i);
        if (is_low)  price_lows.push_back(i);
    }
    // ============ 2. Фракталы индикатора ============
    std::vector<size_t> ind_highs, ind_lows;
    for (size_t i = start; i <= last_visible; ++i) {
        bool is_high = true, is_low = true;
        for (size_t k = 1; k <= fk; ++k) {
            if (ind[i] <= ind[i - k] || ind[i] <= ind[i + k]) is_high = false;
            if (ind[i] >= ind[i - k] || ind[i] >= ind[i + k]) is_low  = false;
        }
        if (is_high) ind_highs.push_back(i);
        if (is_low)  ind_lows.push_back(i);
    }
    auto find_nearest = [&](size_t target, const std::vector<size_t>& candidates) -> long {
        long best = -1;
        long best_dist = LONG_MAX;
        for (size_t c : candidates) {
            long dist = std::labs(static_cast<long>(c) - static_cast<long>(target));
            if (dist < best_dist && dist <= static_cast<long>(fk)) {
                best_dist = dist;
                best = static_cast<long>(c);
            }
        }
        return best;
    };
    // ============ 3. Медвежья дивергенция ============
    if (price_highs.size() >= 2 && ind_highs.size() >= 2) {
        size_t p1 = price_highs[price_highs.size() - 2];
        size_t p2 = price_highs.back();
        long i1 = find_nearest(p1, ind_highs);
        long i2 = find_nearest(p2, ind_highs);
        if (i1 >= 0 && i2 >= 0 && i1 != i2 && i1 < i2) {
            if (bars[p2].high > bars[p1].high && ind[i2] < ind[i1]) {
                res.is_divergence = true;
                res.direction = -1;
                res.first_bar = static_cast<int>(p1);
                res.second_bar = static_cast<int>(p2);
                return res;
            }
        }
    }
    // ============ 4. Бычья дивергенция ============
    if (price_lows.size() >= 2 && ind_lows.size() >= 2) {
        size_t p1 = price_lows[price_lows.size() - 2];
        size_t p2 = price_lows.back();
        long i1 = find_nearest(p1, ind_lows);
        long i2 = find_nearest(p2, ind_lows);
        if (i1 >= 0 && i2 >= 0 && i1 != i2 && i1 < i2) {
            if (bars[p2].low < bars[p1].low && ind[i2] > ind[i1]) {
                res.is_divergence = true;
                res.direction = 1;
                res.first_bar = static_cast<int>(p1);
                res.second_bar = static_cast<int>(p2);
                return res;
            }
        }
    }
    return res;
}
}
