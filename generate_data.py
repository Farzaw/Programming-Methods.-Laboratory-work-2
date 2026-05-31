#!/usr/bin/env python3
"""Generate random flight CSV data files of various sizes (100 .. 1 000 000)."""
import os
import random

AIRLINES = [
    "Aeroflot", "S7 Airlines", "Ural Airlines", "Pobeda",
    "Azimuth", "Nordavia", "Yakutia", "UTair",
    "Rossiya", "Red Wings",
]

SIZES = [
    100, 500, 1000, 2000, 5000, 10000, 20000, 50000,
    75000, 100000, 200000, 500000, 1000000,
]


def random_date() -> str:
    month = random.randint(1, 12)
    day = random.randint(1, 28)
    return f"2024-{month:02d}-{day:02d}"


def random_time() -> str:
    return f"{random.randint(0, 23):02d}:{random.randint(0, 59):02d}"


def main() -> None:
    os.makedirs("data", exist_ok=True)
    for n in SIZES:
        path = f"data/flights_{n}.csv"
        with open(path, "w", encoding="utf-8") as fout:
            fout.write("flightNumber,airline,arrivalDate,arrivalTime,passengers\n")
            for _ in range(n):
                flight_num = f"SU{random.randint(100, 9999)}"
                airline = random.choice(AIRLINES)
                fout.write(
                    f"{flight_num},{airline},{random_date()},{random_time()},"
                    f"{random.randint(50, 350)}\n"
                )
        print(f"Generated {path} ({n} records)")


if __name__ == "__main__":
    main()
