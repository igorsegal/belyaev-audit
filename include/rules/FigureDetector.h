#pragma once
#include "core/Types.h"
#include <vector>
namespace belyaev::rules {
class FigureDetector {
public:
    // fast_line — быстрая линия индикатора
    // slow_line — медленная линия (для MACD(L) передавать ту же, что fast)
    // bars — бары для проверки свечей
    // heavy_trend — 1 вверх, -1 вниз, 0 нет
    static int check_figures(
        const std::vector<double>& fast_line,
        const std::vector<double>& slow_line,
        const std::vector<core::Bar>& bars,
        int heavy_trend
    );
};
}
