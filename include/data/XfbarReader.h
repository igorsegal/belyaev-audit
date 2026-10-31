#pragma once
#include "core/Types.h"
#include <string>
#include <vector>
#include <cstdint>
namespace belyaev::data {
struct XfbarHeader {
    char magic[8]; int32_t version; int32_t record_size; int32_t period_seconds; int32_t digits;
    double point; int64_t bar_count; int64_t first_time; int64_t last_time; std::string symbol;
};
class XfbarReader {
public:
    static bool load(const std::string& path, std::vector<core::Bar>& out, XfbarHeader* header = nullptr);
};
}
