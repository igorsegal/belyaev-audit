#pragma once
#include <vector>
namespace belyaev::levels {
class FiboLevels {
public:
    struct Result {
        double level_146;
        double level_236;
        double level_382;
        double level_500;
        double level_618;
        double level_764;
    };
    // Строит уровни от последнего законченного движения
    // high - максимум, low - минимум
    static Result calculate(double high, double low);
};
}
