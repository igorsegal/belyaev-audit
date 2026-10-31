#pragma once
#include <vector>
#include <string>
#include <cstdint>
namespace belyaev::data {
struct Features {
    std::vector<int64_t> time; std::vector<double> ema1, ema5, ema8, ema13, ema21;
    std::vector<double> cci5, cci8; std::vector<double> rsi5, rsi8;
    std::vector<double> stoch_k, stoch_d; std::vector<double> roc7, roc13;
    std::vector<double> macd_l, macd_g; std::vector<double> plus_dm, minus_dm, adx;
};
class FeatureReader {
public:
    static bool load(const std::string& path, Features& out);
};
}
