#pragma once
#include "core/Types.h"
#include <vector>
#include <string>
namespace belyaev::rules {
class SecondShadowPredictor {
public:
    struct Result {
        int direction = 0;
        int points = 0;
        std::string description;
    };
    static Result predict(bool body_is_white, const std::vector<double>& macd_l, const std::vector<double>& macd_g);
};
}
