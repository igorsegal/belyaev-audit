#include "rules/SecondShadowPredictor.h"
#include <cmath>
namespace belyaev::rules {
SecondShadowPredictor::Result SecondShadowPredictor::predict(
    bool body_is_white, const std::vector<double>& macd_l, const std::vector<double>& macd_g) {
    Result res;
    if (macd_l.size() < 2 || macd_g.size() < 2) {
        res.description = "Мало данных для MACD";
        return res;
    }
    size_t n = macd_l.size() - 1;
    bool macd_l_up = macd_l[n] > macd_l[n-1];
    bool macd_g_up = macd_g[n] > macd_g[n-1];
    int score = 0;
    if (body_is_white && !macd_l_up) score -= 25;
    if (!body_is_white && macd_l_up) score += 25;
    if (body_is_white && !macd_g_up) score -= 25;
    if (!body_is_white && macd_g_up) score += 25;
    res.points = score;
    if (score > 0) { res.direction = 1; res.description = "Вторая тень вверх"; }
    else if (score < 0) { res.direction = -1; res.description = "Вторая тень вниз"; }
    else { res.direction = 0; res.description = "Вторая тень не выражена"; }
    return res;
}
}
