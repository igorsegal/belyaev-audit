import csv
from collections import defaultdict
TRADES_FILE = r"D:\AHexaTrader\2026.07.02_Belyaev\trades.csv"
# Собираем только сделки с confidence == 100 (raw_up или raw_down == 100.0)
by_symbol = defaultdict(list)
with open(TRADES_FILE, 'r', encoding='utf-8') as f:
    reader = csv.DictReader(f)
    for row in reader:
        conf = float(row['confidence'])
        if conf < 99.5:
            continue
        sym = row['symbol']
        t = int(row['time'])
        predicted = row['predicted_direction']
        actual = row['actual_outcome']
        correct = (predicted == actual)
        by_symbol[sym].append({'time': t, 'correct': correct})
print("=" * 90)
print("АНАЛИЗ СДЕЛОК С RAW=100%")
print("=" * 90)
print(f"{'Символ':<12}{'Сделок':<10}{'Побед':<10}{'Win%':<10}{'Блок1':<10}{'Блок2':<10}{'Блок3':<10}{'Блок4':<10}{'Стабильно?'}")
print("-" * 90)
stable_symbols = []
total_all = 0
wins_all = 0
for sym in sorted(by_symbol.keys()):
    trades = sorted(by_symbol[sym], key=lambda x: x['time'])
    n = len(trades)
    wins = sum(1 for t in trades if t['correct'])
    wr = 100.0 * wins / n if n > 0 else 0.0
    total_all += n
    wins_all += wins
    if n < 10:
        continue
    # Разбиваем на 4 блока по времени (равные по числу сделок)
    block_size = max(1, n // 4)
    blocks = []
    for i in range(4):
        start = i * block_size
        end = (i + 1) * block_size if i < 3 else n
        chunk = trades[start:end]
        if len(chunk) == 0:
            blocks.append(0.0)
            continue
        chunk_wins = sum(1 for t in chunk if t['correct'])
        blocks.append(100.0 * chunk_wins / len(chunk))
    # Стабильность: все блоки >= 50% И общий >= 55%
    stable = (all(b >= 50.0 for b in blocks) and wr >= 55.0)
    if stable:
        stable_symbols.append((sym, n, wr, blocks))
    marker = "ДА" if stable else ""
    print(f"{sym:<12}{n:<10}{wins:<10}{wr:<10.1f}"
          f"{blocks[0]:<10.1f}{blocks[1]:<10.1f}{blocks[2]:<10.1f}{blocks[3]:<10.1f}{marker}")
print("-" * 90)
overall_wr = 100.0 * wins_all / total_all if total_all > 0 else 0
print(f"{'ВСЕГО':<12}{total_all:<10}{wins_all:<10}{overall_wr:<10.1f}")
print("=" * 90)
print("\nСТАБИЛЬНЫЕ СИМВОЛЫ (все 4 блока >= 50% И общий winrate >= 55%):")
print("-" * 90)
if not stable_symbols:
    print("НЕТ.")
else:
    for sym, n, wr, blocks in stable_symbols:
        print(f"  {sym}: {n} сделок, winrate {wr:.1f}%, блоки: "
              f"{blocks[0]:.0f}/{blocks[1]:.0f}/{blocks[2]:.0f}/{blocks[3]:.0f}")
print("\nИТОГ:")
print("-" * 90)
if not stable_symbols:
    print("[X] Ни одного стабильного символа. Методика не даёт устойчивого edge.")
    print("    Рекомендация: переход к варианту B (признать, что стратегия не работает).")
elif len(stable_symbols) < 3:
    print(f"[!] Только {len(stable_symbols)} стабильных символа. Слишком мало для портфеля.")
    print("    Можно попробовать расширить выборку на raw >= 90%.")
else:
    print(f"[+] {len(stable_symbols)} стабильных символов. Есть база для портфеля.")
    print("    Следующий шаг: walk-forward с оптимизацией на этих символах.")
