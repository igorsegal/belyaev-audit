#include <iostream>
#include <filesystem>
#include <vector>
#include "data/XfbarReader.h"
#include "data/XfbarWriter.h"
#include "data/BarAggregator.h"
namespace fs = std::filesystem;
using namespace belyaev;
int main() {
    std::string raw_dir = "D:\\AHexaTrader\\1DataFiles\\raw";
    std::cout << "Сканирую: " << raw_dir << std::endl;
    int count_w1 = 0, count_mn1 = 0, count_err = 0;
    for (const auto& dir_entry : fs::directory_iterator(raw_dir)) {
        if (!dir_entry.is_directory()) continue;
        std::string symbol = dir_entry.path().filename().string();
        fs::path d1_path = dir_entry.path() / (symbol + "_D1.bin");
        if (!fs::exists(d1_path)) {
            // std::cout << "[ПРОПУСК] Нет D1 для " << symbol << std::endl;
            continue;
        }
        std::vector<core::Bar> d1_bars;
        data::XfbarHeader header;
        if (!data::XfbarReader::load(d1_path.string(), d1_bars, &header)) {
            std::cout << "[ОШИБКА] Не удалось прочитать " << d1_path << std::endl;
            count_err++;
            continue;
        }
        if (d1_bars.empty()) continue;
        // Агрегация в W1
        auto w1_bars = data::BarAggregator::to_weekly(d1_bars);
        data::XfbarHeader w1_header = header;
        w1_header.period_seconds = 604800; // 7 дней
        w1_header.symbol = symbol;
        fs::path w1_path = dir_entry.path() / (symbol + "_W1.bin");
        if (data::XfbarWriter::save(w1_path.string(), w1_header, w1_bars)) {
            count_w1++;
        }
        // Агрегация в MN1
        auto mn1_bars = data::BarAggregator::to_monthly(d1_bars);
        data::XfbarHeader mn1_header = header;
        mn1_header.period_seconds = 2592000; // 30 дней
        mn1_header.symbol = symbol;
        fs::path mn1_path = dir_entry.path() / (symbol + "_MN1.bin");
        if (data::XfbarWriter::save(mn1_path.string(), mn1_header, mn1_bars)) {
            count_mn1++;
        }
        std::cout << "[OK] " << symbol << " | D1: " << d1_bars.size() 
                  << " -> W1: " << w1_bars.size() 
                  << ", MN1: " << mn1_bars.size() << std::endl;
    }
    std::cout << "\n=== ГОТОВО ===" << std::endl;
    std::cout << "Создано W1 файлов: " << count_w1 << std::endl;
    std::cout << "Создано MN1 файлов: " << count_mn1 << std::endl;
    std::cout << "Ошибок: " << count_err << std::endl;
    return 0;
}
