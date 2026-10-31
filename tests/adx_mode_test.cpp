#include "engine/BelyaevPipeline.h"
#include <iostream>
#include <string>
#include <vector>

namespace {
using belyaev::core::Bar;
using belyaev::data::Features;

struct AdxCase {
    double prev_adx;
    double curr_adx;
    std::string expected_mode;
};

Features make_features(double prev_adx, double curr_adx) {
    Features f;
    const std::vector<double> zeros(2, 0.0);
    const std::vector<int64_t> times = {0, 1};

    f.time = times;
    f.ema1 = zeros;
    f.ema5 = zeros;
    f.ema8 = zeros;
    f.ema13 = zeros;
    f.ema21 = zeros;
    f.cci5 = zeros;
    f.cci8 = zeros;
    f.rsi5 = zeros;
    f.rsi8 = zeros;
    f.stoch_k = zeros;
    f.stoch_d = zeros;
    f.roc7 = zeros;
    f.roc13 = zeros;
    f.macd_l = zeros;
    f.macd_g = zeros;
    f.plus_dm = zeros;
    f.minus_dm = zeros;
    f.adx = {prev_adx, curr_adx};
    return f;
}

std::string normalize_mode(const std::string& mode) {
    if (mode == "ADX < 20 (неопределенность)") return "UNCERTAIN";
    if (mode == "FLAT (CCI+DM)") return "FLAT";
    if (mode == "TREND (MACD+EMA)") return "TREND";
    return mode;
}

bool run_case(const AdxCase& test_case) {
    Features features = make_features(test_case.prev_adx, test_case.curr_adx);
    std::vector<Bar> bars(2);
    bars[0] = {0, 100.0, 110.0, 90.0, 100.0, 0, 0, 0};
    bars[1] = {1, 101.0, 111.0, 91.0, 101.0, 0, 0, 0};

    auto node = belyaev::engine::BelyaevPipeline::process_tf("ADX_TEST", features, bars, 1, 0);
    const std::string actual_mode = normalize_mode(node.mode);
    const std::string expected_mode = test_case.expected_mode;

    std::cout << "CASE prevADX=" << test_case.prev_adx
              << " currADX=" << test_case.curr_adx
              << " EXPECT=" << expected_mode
              << " ACTUAL=" << actual_mode
              << " => " << ((actual_mode == expected_mode) ? "PASS" : "FAIL")
              << "\n";
    return actual_mode == expected_mode;
}

}  // namespace

int main() {
    const std::vector<AdxCase> cases = {
        {30.0, 30.0, "UNCERTAIN"},
        {30.0, 35.0, "TREND"},
        {35.0, 30.0, "FLAT"},
        {15.0, 18.0, "UNCERTAIN"},
        {20.0, 20.0, "UNCERTAIN"}
    };

    int failed = 0;
    for (const auto& c : cases) {
        if (!run_case(c)) {
            ++failed;
        }
    }

    std::cout << "SUMMARY: failures=" << failed << " of " << cases.size() << "\n";
    return failed == 0 ? 0 : 1;
}
