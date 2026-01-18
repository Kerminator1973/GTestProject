#include <array>
#include <cstdint>

using U8 = std::uint8_t;

// Полином CRC‑8 (например, 0x07 – CRC‑8‑ATM)
constexpr U8 POLY = 0x07;

// Генерация таблицы в compile‑time
static constexpr std::array<U8, 256> generateTable() {
    std::array<U8, 256> table{};

    for (std::size_t i = 0; i < table.size(); ++i) {
        U8 crc = static_cast<U8>(i);
        for (int bit = 0; bit < 8; ++bit) {
            if (crc & 0x80) {
                crc = static_cast<U8>((crc << 1) ^ POLY);
            } else {
                crc <<= 1;
            }
        }
        table[i] = crc;
    }
    return table;
}

// Таблица доступна как constexpr‑константа
static constexpr std::array<U8, 256> CRC_TABLE = generateTable();

// Пример функции вычисления CRC‑8 с использованием таблицы
constexpr U8 crc8(const std::uint8_t* data, std::size_t length) {
    U8 crc = 0x00;                     // начальное значение
    for (std::size_t i = 0; i < length; ++i) {
        crc = CRC_TABLE[crc ^ data[i]];
    }
    return crc;
}

// Тест‑пример (можно собрать как исполняемый файл)
#include <iostream>

int main() {
    const std::uint8_t msg[] = {0x01, 0x02, 0x03, 0x04};
    constexpr std::size_t len = sizeof(msg) / sizeof(msg[0]);

    // Вычисление CRC‑8 в рантайме (таблица уже готова)
    U8 result = crc8(msg, len);
    std::cout << "CRC8 = 0x" << std::hex << static_cast<int>(result) << '\n';
    return 0;
}
