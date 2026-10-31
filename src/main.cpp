#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "data/FeatureReader.h"
#include "data/XfbarReader.h"
#include "engine/BelyaevPipeline.h"
using namespace belyaev;
int main() {
    std::string sym = "EURUSD";
    std::string base_raw = "D:\\AHexaTrader\\1DataFiles\\raw\\" + sym + "\\";
    std::string base_cache = "D:\\AHexaTrader\\1DataFiles\\cache\\";
    data::Features mn1_f, w1_f, d1_f;
    if (!data::FeatureReader::load(base_cache + sym + "_MN1_features.bin", mn1_f)) return 1;
    if (!data::FeatureReader::load(base_cache + sym + "_W1_features.bin", w1_f)) return 1;
    if (!data::FeatureReader::load(base_cache + sym + "_D1_features.bin", d1_f)) return 1;
    std::vector<core::Bar> mn1_b, w1_b, d1_b;
    if (!data::XfbarReader::load(base_raw + sym + "_MN1.bin", mn1_b)) return 1;
    if (!data::XfbarReader::load(base_raw + sym + "_W1.bin", w1_b)) return 1;
    if (!data::XfbarReader::load(base_raw + sym + "_D1.bin", d1_b)) return 1;
    if (mn1_f.time.empty() || w1_f.time.empty() || d1_f.time.empty()) { std::cerr << "Пустой кэш" << std::endl; return 1; }
    if (mn1_b.empty() || w1_b.empty() || d1_b.empty()) { std::cerr << "Пустые бары" << std::endl; return 1; }
    size_t mn1_idx = std::min(mn1_f.time.size(), mn1_b.size()) - 1;
    size_t w1_idx  = std::min(w1_f.time.size(), w1_b.size()) - 1;
    size_t d1_idx  = std::min(d1_f.time.size(), d1_b.size()) - 1;
    std::cout << "MN1: f=" << mn1_f.time.size() << ", b=" << mn1_b.size() << std::endl;
    std::cout << "W1:  f=" << w1_f.time.size() << ", b=" << w1_b.size() << std::endl;
    std::cout << "D1:  f=" << d1_f.time.size() << ", b=" << d1_b.size() << std::endl;
    auto month = engine::BelyaevPipeline::process_tf("MN1", mn1_f, mn1_b, mn1_idx, 0);
    std::cout << "\n[МЕСЯЦ] Режим: " << month.mode << " | " << month.body_forecast.verdict << std::endl;
    int heavy_for_week = month.is_uncertain ? 0 : month.trend_direction;
    auto week = engine::BelyaevPipeline::process_tf("W1", w1_f, w1_b, w1_idx, heavy_for_week);
    std::cout << "[НЕДЕЛЯ] Режим: " << week.mode << " | " << week.body_forecast.verdict << std::endl;
    int heavy_for_day = week.is_uncertain ? 0 : week.trend_direction;
    auto day = engine::BelyaevPipeline::process_tf("D1", d1_f, d1_b, d1_idx, heavy_for_day);
    std::cout << "[ДЕНЬ]  Режим: " << day.mode << " | " << day.body_forecast.verdict << std::endl;
    std::cout << "  Паттерн: " << (day.candle_pattern.pattern_name.empty() ? "нет" : day.candle_pattern.pattern_name) << std::endl;
    std::cout << "\n=== ИТОГ ===" << std::endl;
    if (day.is_uncertain) std::cout << "Свеча: НЕОПРЕДЕЛЕНА" << std::endl;
    else std::cout << "Свеча: " << (day.trend_direction == 1 ? "БЕЛАЯ" : "ЧЕРНАЯ") << std::endl;
    std::cout << "ВВЕРХ: " << day.body_forecast.up_probability << "% | ВНИЗ: " << day.body_forecast.down_probability << "%" << std::endl;
    return 0;
}
