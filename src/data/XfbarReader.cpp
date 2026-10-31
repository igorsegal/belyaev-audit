#include "data/XfbarReader.h"
#include <fstream>
#include <iostream>
namespace belyaev::data {
bool XfbarReader::load(const std::string& path, std::vector<core::Bar>& out, XfbarHeader* header) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return false;
    XfbarHeader h;
    file.read(h.magic, 8);
    file.read(reinterpret_cast<char*>(&h.version), 4);
    file.read(reinterpret_cast<char*>(&h.record_size), 4);
    file.read(reinterpret_cast<char*>(&h.period_seconds), 4);
    file.read(reinterpret_cast<char*>(&h.digits), 4);
    file.read(reinterpret_cast<char*>(&h.point), 8);
    file.read(reinterpret_cast<char*>(&h.bar_count), 8);
    file.read(reinterpret_cast<char*>(&h.first_time), 8);
    file.read(reinterpret_cast<char*>(&h.last_time), 8);
    int32_t symbol_len; file.read(reinterpret_cast<char*>(&symbol_len), 4);
    h.symbol.resize(symbol_len); file.read(&h.symbol[0], symbol_len);
    if (std::string(h.magic, 8) != "XFBAR001") return false;
    out.resize(h.bar_count);
    for (int64_t i = 0; i < h.bar_count; ++i) {
        file.read(reinterpret_cast<char*>(&out[i].time), 8);
        file.read(reinterpret_cast<char*>(&out[i].open), 8);
        file.read(reinterpret_cast<char*>(&out[i].high), 8);
        file.read(reinterpret_cast<char*>(&out[i].low), 8);
        file.read(reinterpret_cast<char*>(&out[i].close), 8);
        file.read(reinterpret_cast<char*>(&out[i].tick_volume), 8);
        file.read(reinterpret_cast<char*>(&out[i].spread), 4);
        file.read(reinterpret_cast<char*>(&out[i].real_volume), 8);
    }
    if (header) *header = h;
    return true;
}
}
