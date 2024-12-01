#include "SerialPort.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <windows.h>
#include <string>

HANDLE hConsole;
std::string port;
int baudRate;

bool SerialCommunication::connect(const std::string& portName, int baudRate) {
    std::wstring widePortName(portName.begin(), portName.end());
    LPCWSTR lpPortName = widePortName.c_str();

    hSerial = CreateFile(lpPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "The serial port is not open: " << portName << std::endl;
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
    char buffer[256];
    DWORD bytesRead;
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
