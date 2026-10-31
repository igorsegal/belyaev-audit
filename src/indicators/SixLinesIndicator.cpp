#include "indicators/SixLinesIndicator.h"
namespace belyaev::indicators {
SixLinesIndicator::Result SixLinesIndicator::analyze(
    const std::vector<double>& stoch_k,
    const std::vector<double>& stoch_d,
    const std::vector<double>& cci_fast,
    const std::vector<double>& cci_slow,
    const std::vector<double>& rsi_fast,
    const std::vector<double>& rsi_slow
) {
    Result res;
    if (stoch_k.size() < 2 || cci_fast.size() < 2 || rsi_fast.size() < 2) {
        res.description = "Мало данных";
        return res;
    }
    size_t n = stoch_k.size() - 1;
    // Определяем направление каждой из 6 линий (по наклону последнего бара)
    auto dir = [](double curr, double prev) -> int {
        if (curr > prev) return 1;
        if (curr < prev) return -1;
        return 0;
    };
    int dirs[6] = {
        dir(stoch_k[n], stoch_k[n-1]),   // fast K
        dir(stoch_d[n], stoch_d[n-1]),   // slow D
        dir(cci_fast[n], cci_fast[n-1]), // fast CCI
        dir(cci_slow[n], cci_slow[n-1]), // slow CCI
        dir(rsi_fast[n], rsi_fast[n-1]), // fast RSI
        dir(rsi_slow[n], rsi_slow[n-1])  // slow RSI
    };
    // Правило 1: Все 6 в одну сторону
    int ups = 0, downs = 0;
    for (int d : dirs) {
        if (d > 0) ups++;
        else if (d < 0) downs++;
    }
    if (ups == 6) {
        res.direction = 1;
        res.points = 25;
        res.description = "Все 6 линий вверх (сильный сигнал)";
        return res;
    }
    if (downs == 6) {
        res.direction = -1;
        res.points = -25;
        res.description = "Все 6 линий вниз (сильный сигнал)";
        return res;
    }
    // Правило 2: Хотя бы одна медленная против остальных → 0 баллов
    // Медленные: Stoch_D (1), CCI_slow (3), RSI_slow (5)
    int slow_dirs[3] = { dirs[1], dirs[3], dirs[5] };
    int slow_ups = 0, slow_downs = 0;
    for (int d : slow_dirs) {
        if (d > 0) slow_ups++;
        else if (d < 0) slow_downs++;
    }
    bool slow_conflict = (slow_ups > 0 && slow_downs > 0);
    if (slow_conflict) {
        res.direction = 0;
        res.points = 0;
        res.description = "Конфликт медленных линий (0 баллов)";
        return res;
    }
    // Правило 3: 1-2 быстрые против → берем направление медленных
    int slow_dir = (slow_ups > slow_downs) ? 1 : -1;
    res.direction = slow_dir;
    res.points = (slow_dir > 0) ? 25 : -25;
    res.description = (slow_dir > 0) 
        ? "Медленные вверх, быстрые частично против (25↑)" 
        : "Медленные вниз, быстрые частично против (25↓)";
    return res;
}
}
