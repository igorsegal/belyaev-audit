#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <climits>
#include "data/FeatureReader.h"
#include "data/XfbarReader.h"
#include "engine/BelyaevPipeline.h"
namespace fs = std::filesystem;
using namespace belyaev;
size_t find_last_le(const std::vector<int64_t>& times, int64_t target) {
    if (times.empty()) return SIZE_MAX;
    if (target < times.front()) return SIZE_MAX;
    auto it = std::upper_bound(times.begin(), times.end(), target);
    if (it == times.begin()) return SIZE_MAX;
    return static_cast<size_t>(std::distance(times.begin(), it) - 1);
}
struct SymbolStats {
    std::string symbol;
    int total = 0;
    int wins = 0;
    int losses = 0;
    int high_conf_total = 0;
    int high_conf_wins = 0;
    double sum_confidence = 0.0;
};
int main() {
    std::string raw_base = "D:\\AHexaTrader\\1DataFiles\\raw\\";
    std::string cache_base = "D:\\AHexaTrader\\1DataFiles\\cache\\";
    std::string output_csv = "D:\\AHexaTrader\\2026.07.02_Belyaev\\backtest_results.csv";
    std::string trades_csv = "D:\\AHexaTrader\\2026.07.02_Belyaev\\trades.csv";
    const int bars_to_check = 100;
    std::vector<std::string> symbols;
    if (!fs::exists(cache_base)) { std::cerr << "Кэш не найден" << std::endl; return 1; }
    for (const auto& entry : fs::directory_iterator(cache_base)) {
        if (!entry.is_regular_file()) continue;
        std::string fname = entry.path().filename().string();
        std::string suffix = "_D1_features.bin";
        if (fname.size() > suffix.size() && fname.substr(fname.size() - suffix.size()) == suffix) {
            symbols.push_back(fname.substr(0, fname.size() - suffix.size()));
        }
    }
    std::sort(symbols.begin(), symbols.end());
    std::cout << "Найдено символов: " << symbols.size() << std::endl;
    // Открываем trades.csv для потоковой записи
    std::ofstream trades(trades_csv);
    trades << "symbol,time,raw_up_prob,raw_down_prob,predicted_direction,actual_outcome,confidence\n";
    trades << std::fixed << std::setprecision(2);
    std::vector<SymbolStats> all_stats;
    all_stats.reserve(symbols.size());
    int processed = 0;
    int total_symbols = static_cast<int>(symbols.size());
    if (total_symbols == 0) { std::cerr << "Нет символов" << std::endl; return 1; }
    for (const auto& sym : symbols) {
        data::Features d1f, w1f, mn1f;
        if (!data::FeatureReader::load(cache_base + sym + "_D1_features.bin", d1f)) { processed++; continue; }
        if (!data::FeatureReader::load(cache_base + sym + "_W1_features.bin", w1f)) { processed++; continue; }
        if (!data::FeatureReader::load(cache_base + sym + "_MN1_features.bin", mn1f)) { processed++; continue; }
        std::vector<core::Bar> d1b, w1b, mn1b;
        if (!data::XfbarReader::load(raw_base + sym + "\\" + sym + "_D1.bin", d1b)) { processed++; continue; }
        if (!data::XfbarReader::load(raw_base + sym + "\\" + sym + "_W1.bin", w1b)) { processed++; continue; }
        if (!data::XfbarReader::load(raw_base + sym + "\\" + sym + "_MN1.bin", mn1b)) { processed++; continue; }
        size_t d1n = std::min(d1f.time.size(), d1b.size());
        size_t w1n = std::min(w1f.time.size(), w1b.size());
        size_t mn1n = std::min(mn1f.time.size(), mn1b.size());
        if (d1n < 10 || w1n < 5 || mn1n < 3) { processed++; continue; }
        SymbolStats st;
        st.symbol = sym;
        size_t start_i = (d1n > static_cast<size_t>(bars_to_check)) ? (d1n - bars_to_check - 1) : 1;
        for (size_t i = start_i; i < d1n - 1; ++i) {
            int64_t t = d1f.time[i];
            size_t mn1_i = find_last_le(mn1f.time, t);
            size_t w1_i  = find_last_le(w1f.time, t);
            if (mn1_i == SIZE_MAX || w1_i == SIZE_MAX) continue;
            if (mn1_i >= mn1n || w1_i >= w1n) continue;
            auto month = engine::BelyaevPipeline::process_tf("MN1", mn1f, mn1b, mn1_i, 0);
            int heavy_week = month.is_uncertain ? 0 : month.trend_direction;
            auto week = engine::BelyaevPipeline::process_tf("W1", w1f, w1b, w1_i, heavy_week);
            int heavy_day = week.is_uncertain ? 0 : week.trend_direction;
            auto day = engine::BelyaevPipeline::process_tf("D1", d1f, d1b, i, heavy_day);
            if (day.is_uncertain) continue;
            bool real_white = d1b[i+1].close > d1b[i+1].open;
            bool pred_white = (day.trend_direction == 1);
            bool correct = (pred_white == real_white);
            double up = std::clamp(day.body_forecast.up_probability, 0.0, 100.0);
            double down = std::clamp(day.body_forecast.down_probability, 0.0, 100.0);
            double conf = std::max(up, down);
            st.total++;
            if (correct) st.wins++; else st.losses++;
            st.sum_confidence += conf;
            if (conf > 75.0) {
                st.high_conf_total++;
                if (correct) st.high_conf_wins++;
            }
            // Пишем каждую сделку в trades.csv
            trades << sym << "," << t << ","
                   << up << "," << down << ","
                   << (pred_white ? "WHITE" : "BLACK") << ","
                   << (real_white ? "WHITE" : "BLACK") << ","
                   << conf << "\n";
        }
        if (st.total > 0) all_stats.push_back(st);
        processed++;
        if (processed % 10 == 0 || processed == total_symbols) {
            int pct = processed * 100 / total_symbols;
            int width = 40;
            int filled = pct * width / 100;
            std::cout << "\r[";
            for (int w = 0; w < width; ++w) std::cout << ((w < filled) ? '#' : '-');
            std::cout << "] " << std::setw(3) << pct << "% (" << processed << "/" << total_symbols << ")  " << std::flush;
        }
    }
    std::cout << std::endl;
    trades.close();
    // Сохраняем backtest_results.csv
    std::ofstream csv(output_csv);
    csv << "symbol,total,wins,losses,win_rate,avg_confidence,high_conf_total,high_conf_wins,high_conf_win_rate\n";
    csv << std::fixed << std::setprecision(2);
    std::sort(all_stats.begin(), all_stats.end(), [](const SymbolStats& a, const SymbolStats& b) {
        double wr_a = (a.total > 0) ? (double)a.wins / a.total : 0.0;
        double wr_b = (b.total > 0) ? (double)b.wins / b.total : 0.0;
        if (wr_a != wr_b) return wr_a > wr_b;
        return a.total > b.total;
    });
    int total_pred = 0, total_wins = 0;
    for (const auto& s : all_stats) {
        double wr = (s.total > 0) ? 100.0 * s.wins / s.total : 0.0;
        double avg_conf = (s.total > 0) ? s.sum_confidence / s.total : 0.0;
        double hc_wr = (s.high_conf_total > 0) ? 100.0 * s.high_conf_wins / s.high_conf_total : 0.0;
        csv << s.symbol << "," << s.total << "," << s.wins << "," << s.losses << ","
            << wr << "," << avg_conf << ","
            << s.high_conf_total << "," << s.high_conf_wins << "," << hc_wr << "\n";
        total_pred += s.total;
        total_wins += s.wins;
    }
    csv.close();
    std::cout << "\n=== ИТОГИ ===" << std::endl;
    std::cout << "Всего прогнозов: " << total_pred << std::endl;
    std::cout << "Побед: " << total_wins << " (" << (total_pred > 0 ? 100.0 * total_wins / total_pred : 0.0) << "%)" << std::endl;
    std::cout << "\nСохранено:" << std::endl;
    std::cout << "  " << output_csv << std::endl;
    std::cout << "  " << trades_csv << std::endl;
    return 0;
}
