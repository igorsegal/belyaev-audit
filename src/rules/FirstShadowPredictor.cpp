#include "rules/FirstShadowPredictor.h"
#include <cmath>
#include <string>
#include <algorithm>
namespace belyaev::rules {
FirstShadowPredictor::Result FirstShadowPredictor::predict(
    bool body_is_white,
    const std::vector<double>& stoch_k,
    const std::vector<double>& rsi_fast,
    const std::vector<double>& roc7,
    const std::vector<double>& ema1,
    const std::vector<double>& ema5,
    const std::vector<double>& ema8,
    int six_lines_direction,
    int six_lines_points
) {
    Result res;
    if (stoch_k.size() < 2 || rsi_fast.size() < 2 || roc7.size() < 2 ||
        ema1.size() < 2 || ema5.size() < 2 || ema8.size() < 2) {
        res.description = "Мало данных";
        return res;
    }
    int against = 0;
    int total_signals = 0;
    int body_dir = body_is_white ? 1 : -1;
    // === Компонент 1: Stoch + RSI выше/ниже 50% ===
    // Индексируем каждый вектор от конца (последний элемент)
    double stoch_last = stoch_k.back();
    double rsi_last = rsi_fast.back();
    bool stoch_above = stoch_last > 50.0;
    bool rsi_above = rsi_last > 50.0;
    int comp1_dir = 0;
    if (stoch_above && rsi_above) comp1_dir = 1;
    else if (!stoch_above && !rsi_above) comp1_dir = -1;
    if (comp1_dir != 0) {
        total_signals++;
        if (comp1_dir != body_dir) against++;
    }
    // === Компонент 2: ROC(7) > 0 -> вверх, < 0 -> вниз ===
    double roc_last = roc7.back();
    int comp2_dir = 0;
    if (roc_last > 0) comp2_dir = 1;
    else if (roc_last < 0) comp2_dir = -1;
    if (comp2_dir != 0) {
        total_signals++;
        if (comp2_dir != body_dir) against++;
    }
    // === Компонент 3: EMA местная работа (наклон EMA5) ===
    // Индексируем от конца: ema5[last] и ema5[last-1]
    int comp3_dir = 0;
    double ema5_cur = ema5.back();
    double ema5_prev = ema5[ema5.size() - 2];
    if (ema5_cur > ema5_prev) comp3_dir = 1;
    else if (ema5_cur < ema5_prev) comp3_dir = -1;
    if (comp3_dir != 0) {
        total_signals++;
        if (comp3_dir != body_dir) against++;
    }
    // === Компонент 4: Шесть линий (D-04) ===
    if (six_lines_points != 0 && six_lines_direction != 0) {
        total_signals++;
        if (six_lines_direction != body_dir) against++;
    }
    res.signals_against_body = against;
    if (against >= 2) {
        res.has_first_shadow = true;
        res.direction = -body_dir;
        res.description = "Первая тень значимая (" + std::to_string(against)
                        + " из " + std::to_string(total_signals) + " сигналов против тела)";
    } else {
        res.has_first_shadow = false;
        res.direction = 0;
        res.description = "Первая тень мала (" + std::to_string(against)
                        + " из " + std::to_string(total_signals) + " сигналов против тела)";
    }
    return res;
}
}
