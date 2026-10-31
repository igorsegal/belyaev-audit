#include "data/FeatureReader.h"
#include <fstream>
#include <iostream>
namespace belyaev::data {
bool FeatureReader::load(const std::string& path, Features& out) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return false;
    int32_t bar_count = 0; file.read(reinterpret_cast<char*>(&bar_count), 4);
    if (bar_count <= 0) return false;
    auto read_array = [&](std::vector<double>& arr) {
        arr.resize(bar_count);
        for (int i = 0; i < bar_count; ++i) { float v; file.read(reinterpret_cast<char*>(&v), 4); arr[i] = v; }
    };
    out.time.resize(bar_count);
    for (int i = 0; i < bar_count; ++i) file.read(reinterpret_cast<char*>(&out.time[i]), 8);
    read_array(out.ema1); read_array(out.ema5); read_array(out.ema8); read_array(out.ema13); read_array(out.ema21);
    read_array(out.cci5); read_array(out.cci8); read_array(out.rsi5); read_array(out.rsi8);
    read_array(out.stoch_k); read_array(out.stoch_d); read_array(out.roc7); read_array(out.roc13);
    read_array(out.macd_l); read_array(out.macd_g); read_array(out.plus_dm); read_array(out.minus_dm); read_array(out.adx);
    return true;
}
}
