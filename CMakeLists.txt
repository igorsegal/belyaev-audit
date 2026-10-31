cmake_minimum_required(VERSION 3.20)
project(Belyaev_System CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(MSVC)
    add_compile_options(/W4 /permissive- /utf-8)
endif()
add_library(belyaev_core STATIC    src/data/FeatureReader.cpp
    src/data/XfbarReader.cpp
    src/engine/BelyaevPipeline.cpp
    src/indicators/CCI.cpp
    src/indicators/DM.cpp
    src/indicators/EMA.cpp
    src/indicators/MACD.cpp
    src/indicators/ROC.cpp
    src/indicators/RSI.cpp
    src/indicators/SixLinesIndicator.cpp
    src/indicators/Stochastic.cpp
    src/levels/BollingerBands.cpp
    src/levels/CandlePatterns.cpp
    src/levels/FiboLevels.cpp
    src/rules/DivergenceFinder.cpp
    src/rules/FigureDetector.cpp
    src/rules/FirstShadowPredictor.cpp
    src/rules/RunAnalyzer.cpp
    src/rules/ScoreCalculator.cpp
    src/rules/SecondShadowPredictor.cpp
)
target_include_directories(belyaev_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(test_build src/main.cpp)
target_link_libraries(test_build PRIVATE belyaev_core)

add_executable(backtest_all tools/backtest_all.cpp)
target_link_libraries(backtest_all PRIVATE belyaev_core)

