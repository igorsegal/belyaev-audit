#include "levels/FiboLevels.h"
namespace belyaev::levels {
FiboLevels::Result FiboLevels::calculate(double high, double low) {
    Result res;
    double diff = high - low;
    res.level_146 = high - (diff * 0.146);
    res.level_236 = high - (diff * 0.236);
    res.level_382 = high - (diff * 0.382);
    res.level_500 = high - (diff * 0.500);
    res.level_618 = high - (diff * 0.618);
    res.level_764 = high - (diff * 0.764);
    return res;
}
}
