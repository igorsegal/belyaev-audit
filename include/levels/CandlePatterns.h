#pragma once
#include "core/Types.h"
#include <vector>
#include <string>
namespace belyaev::levels {
class CandlePatterns {
public:
    struct Result {
        std::string pattern_name;
        int direction = 0; // 1 = бычий (вверх), -1 = медвежий (вниз), 0 = нет паттерна
    };
    // Ищет паттерны на последних 3-х свечах
    static Result detect(const std::vector<core::Bar>& bars);
};
}
