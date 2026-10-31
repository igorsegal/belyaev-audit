#pragma once
#include "core/Types.h"
#include <vector>
#include <string>
namespace belyaev::rules {
struct ForecastResult {
    double up_probability = 50.0;
    double down_probability = 50.0;
    int total_score = 0;
    int reference_direction = 0;
    bool reference_changed = false;
    bool is_conflict = false;
    std::string verdict;
};
class ScoreCalculator {
public:
    // Один индикатор (для MACD(L), где медленная не рассчитывается)
    static ForecastResult calculate(
        const std::vector<double>& ind,
        const std::vector<core::Bar>& bars,
        int heavy_chart_trend
    );
    // Две линии (CCI5/CCI8, +DM/-DM) — фигуры 5-8 работают
    static ForecastResult calculate_dual(
        const std::vector<double>& fast,
        const std::vector<double>& slow,
        const std::vector<core::Bar>& bars,
        int heavy_chart_trend
    );
    // Слияние двух результатов: (P1+P2)/2, конфликт → 50/50
    static ForecastResult merge(const ForecastResult& a, const ForecastResult& b);
private:
    static ForecastResult apply_reference_rule(int raw_sum, int ref_direction);
};
} // namespace belyaev::rules
