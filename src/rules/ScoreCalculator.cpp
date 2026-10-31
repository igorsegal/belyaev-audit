#include "rules/ScoreCalculator.h"
#include "rules/RunAnalyzer.h"
#include "rules/DivergenceFinder.h"
#include "rules/FigureDetector.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>
namespace belyaev::rules {
ForecastResult ScoreCalculator::apply_reference_rule(int raw_sum, int ref_direction) {
    ForecastResult res;
    res.total_score = raw_sum;
    res.reference_direction = ref_direction;
    res.reference_changed = false;
    res.is_conflict = false;
    // Строгая проверка S == 0
    if (raw_sum == 0) {
        res.up_probability = 50.0;
        res.down_probability = 50.0;
        res.is_conflict = true;
        res.verdict = "S=0 (50/50)";
        return res;
    }
    long long abs_sum_ll = std::llabs(static_cast<long long>(raw_sum));
    int abs_sum = (abs_sum_ll > 1000000) ? 1000000 : static_cast<int>(abs_sum_ll);
    bool sum_against_ref = (raw_sum > 0 && ref_direction < 0) || (raw_sum < 0 && ref_direction > 0);
    if (!sum_against_ref) {
        res.reference_changed = false;
        if (ref_direction > 0) {
            res.up_probability = 100.0; res.down_probability = 0.0; res.verdict = "Бычий (эталон)";
        } else if (ref_direction < 0) {
            res.up_probability = 0.0; res.down_probability = 100.0; res.verdict = "Медвежий (эталон)";
        } else {
            res.up_probability = 50.0; res.down_probability = 50.0; res.is_conflict = true; res.verdict = "Эталон 0";
        }
    } else {
        if (abs_sum >= 100) {
            res.reference_changed = true;
            if (raw_sum > 0) { res.up_probability = 100.0; res.down_probability = 0.0; res.verdict = "Смена: БЫЧИЙ"; }
            else { res.up_probability = 0.0; res.down_probability = 100.0; res.verdict = "Смена: МЕДВЕЖИЙ"; }
        } else {
            res.reference_changed = false;
            double remaining = 100.0 - static_cast<double>(abs_sum);
            if (ref_direction > 0) { res.up_probability = remaining; res.down_probability = 100.0 - remaining; res.verdict = "Ослабленный бычий"; }
            else { res.down_probability = remaining; res.up_probability = 100.0 - remaining; res.verdict = "Ослабленный медвежий"; }
        }
    }
    if (std::abs(res.up_probability - 50.0) < 0.01 && std::abs(res.down_probability - 50.0) < 0.01) res.is_conflict = true;
    return res;
}
ForecastResult ScoreCalculator::calculate(const std::vector<double>& ind, const std::vector<core::Bar>& bars, int heavy_chart_trend) {
    return calculate_dual(ind, ind, bars, heavy_chart_trend);
}
ForecastResult ScoreCalculator::calculate_dual(const std::vector<double>& fast, const std::vector<double>& slow, const std::vector<core::Bar>& bars, int heavy_chart_trend) {
    ForecastResult res;
    if (fast.size() < 5 || slow.size() < 5 || bars.size() < 5) {
        res.up_probability = 50.0; res.down_probability = 50.0; res.is_conflict = true; res.verdict = "Мало данных";
        return res;
    }
    if (heavy_chart_trend > 0) heavy_chart_trend = 1;
    else if (heavy_chart_trend < 0) heavy_chart_trend = -1;
    else heavy_chart_trend = 0;
    RunResult run = RunAnalyzer::analyze(fast, bars, heavy_chart_trend);
    int ref_direction = 0;
    if (run.direction == RunDirection::Up) ref_direction = 1;
    else if (run.direction == RunDirection::Down) ref_direction = -1;
    else {
        res.up_probability = 50.0; res.down_probability = 50.0; res.is_conflict = true; res.verdict = "Флэт";
        return res;
    }
    int score = 0;
    if (!run.is_valid) { if (ref_direction > 0) score -= 50; else score += 50; }
    auto div = DivergenceFinder::find(fast, bars);
    if (div.is_divergence) {
        int dir = (div.direction > 0) ? 1 : (div.direction < 0) ? -1 : 0;
        score += (dir * 50);
    }
    score += FigureDetector::check_figures(fast, slow, bars, heavy_chart_trend);
    if (heavy_chart_trend != 0) {
        bool against_heavy = (ref_direction > 0 && heavy_chart_trend < 0) || (ref_direction < 0 && heavy_chart_trend > 0);
        if (against_heavy) { if (ref_direction > 0) score -= 25; else score += 25; }
    }
    score = std::clamp(score, -1000, 1000);
    return apply_reference_rule(score, ref_direction);
}
ForecastResult ScoreCalculator::merge(const ForecastResult& a, const ForecastResult& b) {
    ForecastResult res;
    res.up_probability = (a.up_probability + b.up_probability) / 2.0;
    res.down_probability = (a.down_probability + b.down_probability) / 2.0;
    res.total_score = a.total_score + b.total_score;
    res.reference_changed = a.reference_changed || b.reference_changed;
    // ФИКС: сохраняем reference_direction, если оба совпадают
    res.reference_direction = (a.reference_direction == b.reference_direction) ? a.reference_direction : 0;
    bool both_5050 = (std::abs(res.up_probability - 50.0) < 0.01 && std::abs(res.down_probability - 50.0) < 0.01);
    // ФИКС: конфликт, если хотя бы один вход был конфликтным ИЛИ результат 50/50
    if (a.is_conflict || b.is_conflict || both_5050) {
        res.is_conflict = true;
        res.verdict = "Конфликт (50/50)";
    } else {
        res.is_conflict = false;
        res.verdict = (res.up_probability > 50.0) ? "Бычий" : "Медвежий";
    }
    return res;
}
}

