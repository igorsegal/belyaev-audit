#pragma once
#include "core/Types.h"
#include <vector>
namespace belyaev::rules {
class DivergenceFinder {
public:
    struct Result {
        bool is_divergence = false;
        int direction = 0; // 1 = вверх (бычье), -1 = вниз (медвежье)
        int first_bar = -1;
        int second_bar = -1;
    };
    // lookback — сколько баров назад искать фракталы
    // fractal_k — сколько баров слева/справа для подтверждения фрактала (обычно 2)
    static Result find(
        const std::vector<double>& ind,
        const std::vector<core::Bar>& bars,
        int lookback = 60,
        int fractal_k = 2
    );
};
}
