#include "serialport.h"
#include <gtest/gtest.h>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ���� ��� �������� ���������� �� �������� �����
TEST(SerialCommunicationTest, ConnectValidPort) {
    SerialCommunication serial;
    // ������ ���������� �� ����� (�������� ���� ���� ���� ����������� ��� �����).
    EXPECT_FALSE(serial.connect("COM999", 9600)); // COM999 - ��������� ����.
}

// ���� �� ����������� �� ��������� �����������
TEST(SerialCommunicationTest, SendMessageWhenNotConnected) {
    SerialCommunication serial;
    std::string response = serial.sendMessage("Test");
    EXPECT_EQ(response, ""); // ������� ������� �����, �� ���� �� ����������.
}

// ���� �� ���'�������
TEST(SerialCommunicationTest, DisconnectWithoutConnection) {
    SerialCommunication serial;
    serial.disconnect();
    SUCCEED(); // ���� ���������, ���� �������� �� ���� ��� ������� disconnect ��� ����������.
}

// ���� �� ������������ ������������
TEST(ConfigTest, LoadValidConfigFile) {
    // ��������� ����������� ����� ������������
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

    // ��������� �������� ����
    std::remove(filename.c_str());
}

TEST(ConfigTest, LoadInvalidConfigFile) {
    // ������������� ��������� ����
    loadConfig("nonexistent_file.json");
    // �� ����'������ ��������� �������� ����� �� �������� �� ������ �����.
    // �������, �� �������� �� ������� ������� �������, ��� �������� ������ ���� �� �������������.
    EXPECT_NE(port, "");
    EXPECT_NE(baudRate, 0);
}

// ���� �� ����������� ���
TEST(SerialCommunicationTest, DrawBoard) {
    SerialCommunication serial;
    std::string boardState = "XOXOXOXO ";
    // ���� ��������, �� ����� ����������� ��� �������
    // (���� �� ������� �� ������������).
    EXPECT_NO_THROW(serial.drawBoard(boardState));
}

// ������� ������� ��� ������� �����
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
