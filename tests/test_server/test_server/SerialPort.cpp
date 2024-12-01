#include "SerialPort.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <windows.h>
#include <string>

HANDLE hConsole;
std::string port;
int baudRate;
#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <string>
#include "SerialPort.h"  // ������� ��������� ���� � ����������� ������� loadConfig
#include <nlohmann/json.hpp>  // ������� ��������� �������� ��� ������ � JSON

using json = nlohmann::json;

// �������� ����
class ConfigTest : public ::testing::Test {
protected:
    // ������� �������� ���� ����� ������ ������
    void SetUp() override {
        port.clear();
        baudRate = 0;
    }

    // ���� �� ����������� ����� ������������
    const std::string configFile = "test_config.json";

    // ������� ��� ��������� ���������������� ����� � ��������� ���������
    void createConfigFile(const std::string& content) {
        std::ofstream file(configFile);
        file << content;
        file.close();
    }

    // �������� ����, �� ������ ���������� � loadConfig
    std::string port;
    int baudRate = 0;
};

// ���� ��� �������, ���� ���� �� ����
TEST_F(ConfigTest, TestLoadConfigFileNotFound) {
    const std::string invalidFile = "non_existent_config.json";

    loadConfig(invalidFile); // �������� ����������� ��������� ����

    // �������, �� ���� � baudRate ���������� ���������
    EXPECT_EQ(port, "");
    EXPECT_EQ(baudRate, 0);
}

// ���� ��� ������� ������������ ������� JSON
TEST_F(ConfigTest, TestLoadConfigInvalidJSON) {
    std::string invalidConfig = R"(
    {
        "Connection": {
            "port": "COM3",
            "baudRate": "not_a_number"
        }
    })";

    createConfigFile(invalidConfig); // ��������� ����������� ��������������� ����

    loadConfig(configFile);  // ����������� ������������

    // �������, �� ���� � baudRate ���������� ���������
    EXPECT_EQ(port, "");
    EXPECT_EQ(baudRate, 0);
}

// ���� ��� �������, ���� ���� ��� baudRate ������ ��� ������
TEST_F(ConfigTest, TestLoadConfigMissingFields) {
    std::string missingFieldsConfig = R"(
    {
        "Connection": {
            "port": "",
            "baudRate": 0
        }
    })";

    createConfigFile(missingFieldsConfig); // ��������� ��������������� ���� � �������� ������

    loadConfig(configFile);  // ����������� ������������

    // �������, �� ���� � baudRate ���������� ���������
    EXPECT_EQ(port, "");
    EXPECT_EQ(baudRate, 0);
}

// ���� ��� ������������ ������� JSON (��������� ���������)
TEST_F(ConfigTest, TestLoadConfigMalformedJSON) {
    std::string malformedConfig = R"(
    {
        "Connection": {
            "port": "COM3"
            "baudRate": 9600
        }
    })"; // ³������ ���� �� ������

    createConfigFile(malformedConfig); // ��������� ����������� ��������������� ����

    loadConfig(configFile);  // ����������� ������������

    // �������, �� ���� � baudRate ���������� ���������
    EXPECT_EQ(port, "");
    EXPECT_EQ(baudRate, 0);
}

// ������� ���� ���� �����
TEST_F(ConfigTest, TearDown) {
    std::remove(configFile.c_str());  // ��������� ���������� ����
}


bool SerialCommunication::connect(const std::string& portName, int baudRate) {
    std::wstring widePortName(portName.begin(), portName.end());
    LPCWSTR lpPortName = widePortName.c_str();

    hSerial = CreateFile(lpPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "The serial port is not open: " << portName << std::endl;
        return false;
    }

    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(dcb);
    dcb.BaudRate = baudRate;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcb)) {
        std::cerr << "The serial port is not open!" << std::endl;
        CloseHandle(hSerial);
        return false;
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        std::cerr << "Communication timeouts could not be set!" << std::endl;
        CloseHandle(hSerial);
        return false;
    }

    return true;
}

void SerialCommunication::disconnect() {
    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
    }
}

std::string SerialCommunication::sendMessage(const std::string& message) {
    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "The serial port is not open!" << std::endl;
        return "";
    }

    DWORD bytesWritten;
    if (!WriteFile(hSerial, message.c_str(), message.size(), &bytesWritten, nullptr)) {
        std::cerr << "Unable to read from the serial port!" << std::endl;
        return "";
    }

    char buffer[256];
    DWORD bytesRead;
    if (!ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, nullptr)) {
        std::cerr << "Unable to read from the serial port!" << std::endl;
        return "";
    }
    buffer[bytesRead] = '\0';

    return std::string(buffer);
}

void SerialCommunication::drawBoard(const std::string& boardState) {
    std::cout << "-------------\n";
    for (int i = 0; i < 3; i++) {
        std::cout << "| ";
        for (int j = 0; j < 3; j++) {
            char cell = boardState[i * 3 + j];
            if (cell == 'X' || cell == 'O') {
                std::cout << cell << " | ";
            }
            else {
                std::cout << " " << " | ";
            }
        }
        std::cout << "\n-------------\n";
    }
}

using json = nlohmann::json;
void loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open configuration file: " << filename << std::endl;
        return;
    }
    try {
        json j;
        file >> j;
        port = j["Connection"]["port"].get<std::string>();
        baudRate = j["Connection"]["baudRate"].get<int>();

        if (port.empty() || baudRate == 0) {
            std::cerr << "Problem reading settings. Verify that the file has the correct format and value." << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing JSON file: " << e.what() << std::endl;
    }
}