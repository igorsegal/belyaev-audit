#include "rules/FigureDetector.h"
#include <cmath>
#include <algorithm>
namespace belyaev::rules {
int FigureDetector::check_figures(
    const std::vector<double>& fast,
    const std::vector<double>& slow,
    const std::vector<core::Bar>& bars,
    int heavy_trend
) {
    if (fast.size() < 7 || slow.size() < 7 || bars.size() < 7) return 0;
    size_t n = fast.size() - 1;
    int score = 0;
    double f0 = fast[n] - fast[n-1];
    double f1 = fast[n-1] - fast[n-2];
    double f2 = fast[n-2] - fast[n-3];
    double s0 = slow[n] - slow[n-1];
    const auto& b_cur = bars[n];
    const auto& b_prev = bars[n-1];
    const auto& b_prev2 = bars[n-2];
    auto is_white = [](const core::Bar& b) { return b.close > b.open; };
    auto is_black = [](const core::Bar& b) { return b.close < b.open; };
    auto body = [](const core::Bar& b) { return std::abs(b.close - b.open); };
    bool fast_up = f0 > 0, fast_down = f0 < 0;
    bool slow_up = s0 > 0, slow_down = s0 < 0;
    bool is_oscillator = (fast[n] >= 0.0 && fast[n] <= 100.0);
    // ===== Этап 1: фигуры 1-14 =====
    if (is_black(b_prev) && is_white(b_cur) && body(b_prev) >= 2.0 * body(b_cur) && heavy_trend > 0) score += 25;
    if (is_white(b_prev) && is_black(b_cur) && body(b_prev) >= 2.0 * body(b_cur) && heavy_trend < 0) score -= 25;
    if (fast[n-1] > slow[n-1] && fast[n] < slow[n]) score -= 50;
    if (fast[n-1] < slow[n-1] && fast[n] > slow[n]) score += 50;
    if (is_white(b_prev) && is_white(b_cur) && f1 > 0 && f0 > 0 && std::abs(f0) < std::abs(f1)) score -= 50;
    if (is_black(b_prev) && is_black(b_cur) && f1 < 0 && f0 < 0 && std::abs(f0) < std::abs(f1)) score += 50;
    if (is_oscillator && fast[n-1] < 80.0 && fast[n] >= 80.0) score += 25;
    if (is_oscillator && fast[n-1] > 20.0 && fast[n] <= 20.0) score -= 25;
    if (fast[n-1] < 0.0 && fast[n] >= 0.0) score += 25;
    if (fast[n-1] > 0.0 && fast[n] <= 0.0) score -= 25;
    // ===== Этап 2: фигуры 15-30 =====
    if (is_white(b_prev) && is_white(b_cur) && body(b_cur) > body(b_prev) && f1 > 0 && f0 > 0 && std::abs(f0) < std::abs(f1)) score -= 50;
    if (is_black(b_prev) && is_black(b_cur) && body(b_cur) > body(b_prev) && f1 < 0 && f0 < 0 && std::abs(f0) < std::abs(f1)) score += 50;
    double avg_body = (body(b_prev) + body(b_cur)) / 2.0;
    bool comparable = avg_body > 0 && std::abs(body(b_prev) - body(b_cur)) / avg_body <= 0.13;
    if (comparable && is_white(b_prev) && is_black(b_cur) && fast_up) score += 50;
    if (comparable && is_black(b_prev) && is_white(b_cur) && fast_down) score -= 50;
    if (is_white(b_prev) && is_black(b_cur) && b_cur.high > b_prev.high && b_cur.low > b_prev.low && fast_down) score -= 50;
    if (is_black(b_prev) && is_white(b_cur) && b_cur.high < b_prev.high && b_cur.low < b_prev.low && fast_up) score += 50;
    if (is_white(b_prev2) && f2 < 0 && is_white(b_prev) && f1 < 0 && fast_up) score += 50;
    if (is_black(b_prev2) && f2 > 0 && is_black(b_prev) && f1 > 0 && fast_down) score -= 50;
    if (f2 > 0 && f1 < 0 && f0 > 0) score -= 50;
    if (f2 < 0 && f1 > 0 && f0 < 0) score += 50;
    if (f2 < 0 && std::abs(f1) < 0.13 && f0 > 0) score -= 50;
    if (f2 > 0 && std::abs(f1) < 0.13 && f0 < 0) score += 50;
    if (f2 < 0 && f1 < 0 && std::abs(f1) < std::abs(f2) && f0 > 0) score -= 50;
    if (f2 > 0 && f1 > 0 && std::abs(f1) < std::abs(f2) && f0 < 0) score += 50;
    // ===== Этап 3: фигуры 31-44 =====
    if (f2 < 0 && f1 > 0 && f0 > 0 && std::abs(f0) > std::abs(f1)) score -= 50;
    if (f2 > 0 && f1 < 0 && f0 < 0 && std::abs(f0) > std::abs(f1)) score += 50;
    if (f2 < 0 && f1 < 0 && std::abs(f1) > std::abs(f2) && f0 > 0) score -= 50;
    if (f2 > 0 && f1 > 0 && std::abs(f1) > std::abs(f2) && f0 < 0) score += 50;
    if (f2 < 0 && f1 > 0 && f0 > 0 && std::abs(f0) < std::abs(f1)) score -= 50;
    if (f2 > 0 && f1 < 0 && f0 < 0 && std::abs(f0) < std::abs(f1)) score += 50;
    if (f2 < 0 && f1 < 0 && f0 < 0 && std::abs(f0) > std::abs(f1) && std::abs(f1) > std::abs(f2)) score += 50;
    if (f2 > 0 && f1 > 0 && f0 > 0 && std::abs(f0) > std::abs(f1) && std::abs(f1) > std::abs(f2)) score -= 50;
    if (f2 < 0 && f1 < 0 && f0 < 0 && std::abs(f0) < std::abs(f1) && std::abs(f1) < std::abs(f2)) score += 50;
    if (f2 > 0 && f1 > 0 && f0 > 0 && std::abs(f0) < std::abs(f1) && std::abs(f1) < std::abs(f2)) score -= 50;
    if (f2 < 0 && f1 < 0 && f0 < 0 && std::abs(f1) > std::abs(f2) && std::abs(f1) > std::abs(f0)) score += 50;
    if (f2 > 0 && f1 > 0 && f0 > 0 && std::abs(f1) > std::abs(f2) && std::abs(f1) > std::abs(f0)) score -= 50;
    if (f2 > 0 && f1 > 0 && f0 > 0 && std::abs(f1) < std::abs(f2) && std::abs(f1) < std::abs(f0)) score -= 50;
    if (f2 < 0 && f1 < 0 && f0 < 0 && std::abs(f1) < std::abs(f2) && std::abs(f1) < std::abs(f0)) score += 50;
    return score;
}
}
