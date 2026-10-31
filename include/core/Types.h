#pragma once
#include <cstdint>
namespace belyaev::core {
    #pragma pack(push, 1)
    struct Bar {
        std::int64_t time = 0;
        double open = 0.0;
        double high = 0.0;
        double low = 0.0;
        double close = 0.0;
        std::int64_t tick_volume = 0;
        std::int32_t spread = 0;
        std::int64_t real_volume = 0;
    };
    #pragma pack(pop)
    enum class OrderSide {
        Buy,
        Sell,
        None
    };
} // namespace belyaev::core
