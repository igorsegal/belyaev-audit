#pragma once
#include "core/Types.h"
#include <vector>
#include <string>
namespace belyaev::rules {
class FirstShadowPredictor {
public:
    struct Result {
        bool has_first_shadow = false;
        int direction = 0;
        int signals_against_body = 0;
        std::string description;
    };
    static Result predict(
        bool body_is_white,
        const std::vector<double>& stoch_k,
        const std::vector<double>& rsi_fast,
        const std::vector<double>& roc7,
        const std::vector<double>& ema1,
        const std::vector<double>& ema5,
        const std::vector<double>& ema8,
        int six_lines_direction,   // +1 вверх, -1 вниз, 0 неопределено
        int six_lines_points       // ±25 или 0 (если неопределено)
    );
};
}
