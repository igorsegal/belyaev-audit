
#include "engine/BelyaevPipeline.h"
#include <algorithm>
#include <cmath>

namespace belyaev::engine {

ForecastNode BelyaevPipeline::process_tf(
    const std::string& name,
    const data::Features& f,
    const std::vector<core::Bar>& bars,
    size_t idx,
    int heavy_trend
) {
    ForecastNode node;
    node.timeframe_name = name;
    node.is_uncertain = true;

    if (heavy_trend > 0) heavy_trend = 1;
    else if (heavy_trend < 0) heavy_trend = -1;
    else heavy_trend = 0;

    if (f.ema1.empty() || f.macd_l.empty() || f.macd_g.empty() ||
        f.stoch_k.empty() || f.rsi5.empty() || f.roc7.empty() ||
        f.ema5.empty() || f.ema8.empty() || f.ema13.empty() ||
        f.cci5.empty() || f.cci8.empty() ||
        f.plus_dm.empty() || f.minus_dm.empty() ||
        f.adx.empty() || bars.empty()) {
        node.mode = "ОШИБКА (нет данных)";
        return node;
    }

    size_t min_size = std::min({
        f.ema1.size(), f.macd_l.size(), f.macd_g.size(),
        f.stoch_k.size(), f.rsi5.size(), f.roc7.size(),
        f.ema5.size(), f.ema8.size(), f.ema13.size(),
        f.cci5.size(), f.cci8.size(),
        f.plus_dm.size(), f.minus_dm.size(),
        f.adx.size(), bars.size()
    });

    if (idx >= min_size || idx < 1) {
        node.mode = "ОШИБКА (idx)";
        return node;
    }

    auto slice = [&](const std::vector<double>& arr) {
        return std::vector<double>(
            arr.begin(),
            arr.begin() + std::min(idx + 1, arr.size())
        );
    };

    std::vector<core::Bar> bars_slice(
        bars.begin(),
        bars.begin() + std::min(idx + 1, bars.size())
    );

    // ADX ниже 20: неопределённость.
    if (f.adx[idx] < 20.0) {
        node.mode = "ADX < 20 (неопределенность)";
        node.body_forecast.up_probability = 50.0;
        node.body_forecast.down_probability = 50.0;
        node.body_forecast.is_conflict = true;
        node.is_uncertain = true;
        return node;
    }

    // FIX: равенство ADX означает неопределённость 50/50.
    if (f.adx[idx] == f.adx[idx - 1]) {
        node.mode = "ADX EQUAL (неопределенность)";
        node.body_forecast.up_probability = 50.0;
        node.body_forecast.down_probability = 50.0;
        node.body_forecast.is_conflict = true;
        node.is_uncertain = true;
        return node;
    }

    bool is_flat = (f.adx[idx] < f.adx[idx - 1]);

    rules::ForecastResult result;

    if (is_flat) {
        node.mode = "FLAT (CCI+DM)";

        auto cci = rules::ScoreCalculator::calculate_dual(
            slice(f.cci5), slice(f.cci8),
            bars_slice, heavy_trend
        );

        auto dm = rules::ScoreCalculator::calculate_dual(
            slice(f.plus_dm), slice(f.minus_dm),
            bars_slice, heavy_trend
        );

        result = rules::ScoreCalculator::merge(cci, dm);
    } else {
        node.mode = "TREND (MACD+EMA)";

        auto macd_l = rules::ScoreCalculator::calculate(
            slice(f.macd_l), bars_slice, heavy_trend
        );

        auto macd_g = rules::ScoreCalculator::calculate(
            slice(f.macd_g), bars_slice, heavy_trend
        );

        auto macd = rules::ScoreCalculator::merge(macd_l, macd_g);

        double ema_up = 50.0;

        if (f.ema5[idx] > f.ema8[idx] &&
            f.ema8[idx] > f.ema13[idx]) {
            ema_up = 100.0;
        } else if (f.ema5[idx] < f.ema8[idx] &&
                   f.ema8[idx] < f.ema13[idx]) {
            ema_up = 0.0;
        }

        rules::ForecastResult ema_res;
        ema_res.up_probability = ema_up;
        ema_res.down_probability = 100.0 - ema_up;

        result = rules::ScoreCalculator::merge(macd, ema_res);
    }

    node.body_forecast = result;

    if (result.is_conflict) {
        node.is_uncertain = true;
    } else {
        node.is_uncertain = false;
        node.trend_direction =
            (result.up_probability > 50.0) ? 1 : -1;
    }

    // Тени (только если не конфликт)
    if (!node.is_uncertain) {
        bool is_white = (node.trend_direction == 1);

        node.first_shadow = rules::FirstShadowPredictor::predict(
            is_white,
            slice(f.stoch_k),
            slice(f.rsi5),
            slice(f.roc7),
            slice(f.ema1),
            slice(f.ema5),
            slice(f.ema8),
            0, 0
        );

        node.second_shadow = rules::SecondShadowPredictor::predict(
            is_white,
            slice(f.macd_l),
            slice(f.macd_g)
        );
    }

    if (bars_slice.size() >= 3) {
        node.candle_pattern =
            levels::CandlePatterns::detect(bars_slice);
    }

    return node;
}

}
