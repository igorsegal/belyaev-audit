import csv
from collections import defaultdict
TRADES_FILE = r"D:\AHexaTrader\2026.07.02_Belyaev\trades.csv"
# Бины: [50, 55), [55, 60), ... [95, 100]
BINS = list(range(50, 101, 5))
bin_stats = defaultdict(lambda: {'total': 0, 'wins': 0})
with open(TRADES_FILE, 'r', encoding='utf-8') as f:
    reader = csv.DictReader(f)
    for row in reader:
        conf = float(row['confidence'])
        predicted = row['predicted_direction']
        actual = row['actual_outcome']
        correct = (predicted == actual)
        # Определяем бин (floor к 5)
        b = int(conf // 5) * 5
        if b < 50: b = 50
        if b > 100: b = 100
        bin_stats[b]['total'] += 1
        if correct:
            bin_stats[b]['wins'] += 1
print("=" * 70)
print("КАЛИБРОВОЧНАЯ ТАБЛИЦА (сырая уверенность vs реальный винрейт)")
print("=" * 70)
print(f"{'Сырая увер.':<15}{'Сделок':<10}{'Побед':<10}{'Реальный win%':<18}{'Врёт на':<10}")
print("-" * 70)
total_all = 0
wins_all = 0
for b in BINS:
    stats = bin_stats.get(b, None)
    if stats is None or stats['total'] == 0:
        continue
    real_wr = 100.0 * stats['wins'] / stats['total']
    avg_raw = b + 2.5  # середина бина
    delta = real_wr - avg_raw
    sign = "+" if delta >= 0 else ""
    total_all += stats['total']
    wins_all += stats['wins']
    print(f"{b}-{b+5}%{'':<8}{stats['total']:<10}{stats['wins']:<10}{real_wr:<18.2f}{sign}{delta:.1f}")
print("-" * 70)
overall_wr = 100.0 * wins_all / total_all if total_all > 0 else 0
print(f"{'ВСЕГО':<15}{total_all:<10}{wins_all:<10}{overall_wr:<18.2f}")
print("=" * 70)
# Анализ
print("\nВЫВОДЫ:")
print("-" * 70)
if overall_wr < 55:
    print(f"[!] Общий винрейт {overall_wr:.1f}% — на уровне монетки.")
    print(f"    Система не имеет статистического преимущества.")
elif overall_wr < 60:
    print(f"[~] Общий винрейт {overall_wr:.1f}% — слабое преимущество.")
    print(f"    Нужна калибровка и отбор символов.")
else:
    print(f"[+] Общий винрейт {overall_wr:.1f}% — есть преимущество.")
    print(f"    Нужно проверить устойчивость на walk-forward.")
print("\nСравнение сырой и реальной уверенности:")
for b in BINS:
    stats = bin_stats.get(b, None)
    if stats is None or stats['total'] < 10:
        continue
    real_wr = 100.0 * stats['wins'] / stats['total']
    avg_raw = b + 2.5
    if abs(real_wr - avg_raw) > 10:
        print(f"  Бин {b}-{b+5}%: сырая ~{avg_raw:.0f}%, реальная {real_wr:.0f}% "
              f"({'ПЕРЕУВЕРЕНА' if real_wr < avg_raw else 'НЕДОУВЕРЕНА'}, сделок {stats['total']})")
