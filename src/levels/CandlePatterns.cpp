#include "levels/CandlePatterns.h"
#include <cmath>
#include <algorithm>
namespace belyaev::levels {
CandlePatterns::Result CandlePatterns::detect(const std::vector<core::Bar>& bars) {
    Result res;
    if (bars.size() < 3) return res;
    size_t n = bars.size() - 1;
    const auto& b0 = bars[n];
    const auto& b1 = bars[n-1];
    const auto& b2 = bars[n-2];
    auto is_white = [](const core::Bar& b) { return b.close > b.open; };
    auto is_black = [](const core::Bar& b) { return b.close < b.open; };
    auto body = [](const core::Bar& b) { return std::abs(b.close - b.open); };
    auto range = [](const core::Bar& b) { return b.high - b.low; };
    // Тени с защитой от битых данных
    auto upper_shadow = [](const core::Bar& b) {
        return std::max(0.0, b.high - std::max(b.open, b.close));
    };
    auto lower_shadow = [](const core::Bar& b) {
        return std::max(0.0, std::min(b.open, b.close) - b.low);
    };
    auto is_doji = [&](const core::Bar& b) {
        double rng = range(b);
        return rng > 1e-9 && body(b) < rng * 0.1;
    };
    // Допуск: от максимального диапазона (не от тела b0)
    double tolerance = std::max({body(b0), body(b1), range(b0), range(b1)}) * 0.1;
    if (tolerance <= 0.0) tolerance = 1e-9;
    // === 1. Молот / Повешенный (6.1.1) ===
    if (body(b0) > 0 && lower_shadow(b0) > 2.0 * body(b0) && upper_shadow(b0) < 0.2 * body(b0)) {
        res.pattern_name = is_white(b0) ? "Молот" : "Повешенный";
        res.direction = is_white(b0) ? 1 : -1;
        return res;
    }
    // === 2. Бычье поглощение (6.1.2) ===
    if (is_black(b1) && is_white(b0) && b0.close > b1.open && b0.open < b1.close) {
        res.pattern_name = "Бычье поглощение";
        res.direction = 1;
        return res;
    }
    // === 3. Медвежье поглощение (6.1.2) ===
    if (is_white(b1) && is_black(b0) && b0.close < b1.open && b0.open > b1.close) {
        res.pattern_name = "Медвежье поглощение";
        res.direction = -1;
        return res;
    }
    // === 4. Завеса из темных облаков (6.1.3) ===
    if (is_white(b1) && is_black(b0) && b0.open > b1.high && b0.close < (b1.open + b1.close) / 2.0) {
        res.pattern_name = "Завеса из темных облаков";
        res.direction = -1;
        return res;
    }
    // === 5. Просвет в облаках (6.1.3) ===
    if (is_black(b1) && is_white(b0) && b0.open < b1.low && b0.close > (b1.open + b1.close) / 2.0) {
        res.pattern_name = "Просвет в облаках";
        res.direction = 1;
        return res;
    }
    // === 6. Утренняя звезда (6.1.4) — с проверкой гэпа ===
    if (is_black(b2) && body(b1) > 0 && body(b1) < body(b2) * 0.3 &&
        b1.high < b2.low &&  // гэп вниз
        is_white(b0) && b0.close > (b2.open + b2.close) / 2.0) {
        res.pattern_name = "Утренняя звезда";
        res.direction = 1;
        return res;
    }
    // === 7. Вечерняя звезда (6.1.4) — с проверкой гэпа ===
    if (is_white(b2) && body(b1) > 0 && body(b1) < body(b2) * 0.3 &&
        b1.low > b2.high &&   // гэп вверх
        is_black(b0) && b0.close < (b2.open + b2.close) / 2.0) {
        res.pattern_name = "Вечерняя звезда";
        res.direction = -1;
        return res;
    }
    // === 8. Додж (6.4) ===
    if (is_doji(b0)) {
        res.pattern_name = "Додж";
        res.direction = 0;
        return res;
    }
    // === 9. Харами (6.2.1) — маленькое тело внутри предыдущего ===
    if (body(b1) > 0 && body(b0) < body(b1) && b0.high <= b1.high && b0.low >= b1.low) {
        res.pattern_name = "Харами";
        res.direction = is_white(b1) ? -1 : 1;
        return res;
    }
    // === 10. Пинцет (6.2.2) ===
    if (std::abs(b0.high - b1.high) < tolerance) {
        res.pattern_name = "Пинцет (вершина)";
        res.direction = -1;
        return res;
    }
    if (std::abs(b0.low - b1.low) < tolerance) {
        res.pattern_name = "Пинцет (основание)";
        res.direction = 1;
        return res;
    }
    // === 11. Захват за пояс (6.2.3) — body(b1) > 0 ===
    if (body(b1) > 0 && is_white(b0) && lower_shadow(b0) < body(b0) * 0.05 && body(b0) > body(b1) * 2.0) {
        res.pattern_name = "Бычий захват за пояс";
        res.direction = 1;
        return res;
    }
    if (body(b1) > 0 && is_black(b0) && upper_shadow(b0) < body(b0) * 0.05 && body(b0) > body(b1) * 2.0) {
        res.pattern_name = "Медвежий захват за пояс";
        res.direction = -1;
        return res;
    }
    // === 12. Две взлетевшие вороны (6.2.4) ===
    if (is_black(b0) && is_black(b1) && b0.open > b1.open && b0.close < b1.close) {
        res.pattern_name = "Две взлетевшие вороны";
        res.direction = -1;
        return res;
    }
    // === 13. Три черные вороны (6.2.5) ===
    if (is_black(b0) && is_black(b1) && is_black(b2) &&
        b0.close < b1.close && b1.close < b2.close) {
        res.pattern_name = "Три черные вороны";
        res.direction = -1;
        return res;
    }
    // === 14. Контратака (6.2.6) — явная проверка контраста ===
    bool contrast_0_1 = (is_white(b0) && is_black(b1)) || (is_black(b0) && is_white(b1));
    if (contrast_0_1 && std::abs(b0.close - b1.close) < tolerance) {
        res.pattern_name = is_white(b0) ? "Бычья контратака" : "Медвежья контратака";
        res.direction = is_white(b0) ? 1 : -1;
        return res;
    }
    // === 15. Три белых солдата (6.3.3) ===
    if (is_white(b0) && is_white(b1) && is_white(b2) &&
        b0.close > b1.close && b1.close > b2.close) {
        res.pattern_name = "Три белых солдата";
        res.direction = 1;
        return res;
    }
    // === 16. Три метода (6.3.2) — body(b1) > 0 ===
    if (body(b1) > 0 && body(b2) > body(b1) * 2.0 && body(b0) > body(b1) * 2.0 &&
        is_white(b2) && is_white(b0) && b0.close > b2.close) {
        res.pattern_name = "Бычья модель Три метода";
        res.direction = 1;
        return res;
    }
    if (body(b1) > 0 && body(b2) > body(b1) * 2.0 && body(b0) > body(b1) * 2.0 &&
        is_black(b2) && is_black(b0) && b0.close < b2.close) {
        res.pattern_name = "Медвежья модель Три метода";
        res.direction = -1;
        return res;
    }
    // === 17. Разделение (6.3.4) — явная проверка контраста ===
    if (contrast_0_1 && std::abs(b0.open - b1.open) < tolerance) {
        res.pattern_name = is_white(b0) ? "Бычье разделение" : "Медвежье разделение";
        res.direction = is_white(b0) ? 1 : -1;
        return res;
    }
    return res;
}
}
