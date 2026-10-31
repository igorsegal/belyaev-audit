#pragma once
#include "data/FeatureReader.h"
#include "core/Types.h"
#include "rules/ScoreCalculator.h"
#include "rules/FirstShadowPredictor.h"
#include "rules/SecondShadowPredictor.h"
#include "levels/CandlePatterns.h"
#include <vector>
#include <string>
#include <cstddef>
namespace belyaev::engine {
struct ForecastNode {
    std::string timeframe_name;
    std::string mode;
    rules::ForecastResult body_forecast;
    rules::FirstShadowPredictor::Result first_shadow;
    rules::SecondShadowPredictor::Result second_shadow;
    levels::CandlePatterns::Result candle_pattern;
    int trend_direction = 0;
    bool is_uncertain = false;
};
class BelyaevPipeline {
public:
    static ForecastNode process_tf(
        const std::string& name,
        const data::Features& f,
        const std::vector<core::Bar>& bars,
        size_t idx,
        int heavy_trend
    );
};
}
