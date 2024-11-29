#include "serialport.h"
#include <gtest/gtest.h>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Тест для перевірки підключення до серійного порту
TEST(SerialCommunicationTest, ConnectValidPort) {
    SerialCommunication serial;
    // Імітуємо підключення до порту (реальний порт може бути недоступний для тесту).
    EXPECT_FALSE(serial.connect("COM999", 9600)); // COM999 - неіснуючий порт.
}

// Тест на відправлення та отримання повідомлення
TEST(SerialCommunicationTest, SendMessageWhenNotConnected) {
    SerialCommunication serial;
    std::string response = serial.sendMessage("Test");
    EXPECT_EQ(response, ""); // Очікуємо порожній рядок, бо порт не підключений.
}

// Тест на роз'єднання
TEST(SerialCommunicationTest, DisconnectWithoutConnection) {
    SerialCommunication serial;
    serial.disconnect();
    SUCCEED(); // Тест проходить, якщо програма не падає при виклику disconnect без підключення.
}

// Тест на завантаження конфігурації
TEST(ConfigTest, LoadValidConfigFile) {
    // Створення тимчасового файлу конфігурації
    std::string filename = "test_config.json";
    json j = {
        {"Connection", {
            {"port", "COM6"},
            {"baudRate", 9600}
        }}
    };
    std::ofstream file(filename);
    file << j.dump(4);
    file.close();

    loadConfig(filename);

    EXPECT_EQ(port, "COM6");
    EXPECT_EQ(baudRate, 9600);

    // Видаляємо тестовий файл
    std::remove(filename.c_str());
}

TEST(ConfigTest, LoadInvalidConfigFile) {
    // Використовуємо неіснуючий файл
    loadConfig("nonexistent_file.json");
    // Не обов'язково перевіряти значення порту та швидкості на порожні рядки.
    // Очікуємо, що програма не викличе критичні помилки, але значення можуть бути за замовчуванням.
    EXPECT_NE(port, "");
    EXPECT_NE(baudRate, 0);
}

// Тест на відображення гри
TEST(SerialCommunicationTest, DrawBoard) {
    SerialCommunication serial;
    std::string boardState = "XOXOXOXO ";
    // Тест перевіряє, що метод викликається без помилок
    // (вивід на консоль не перевіряється).
    EXPECT_NO_THROW(serial.drawBoard(boardState));
}

// Головна функція для запуску тестів
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
