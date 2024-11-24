#include <gtest/gtest.h>
#include "SerialPort.h"  // Your serial communication class
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;
string portName;
int portSpeed;

// Функція для нормалізації рядків (замінює \r на \n)
string normalizeNewlines(const string& str) {
    string normalized = str;
    replace(normalized.begin(), normalized.end(), '\r', '\n');  // Перетворюємо '\r' на '\n'
    return normalized;
}

// Test fixture for serial communication with the Arduino-based Tic-Tac-Toe game
class SerialPortTest : public ::testing::Test {
protected:
    SerialCommunication serial;

    // Initialize the Serial Communication before each test
    void SetUp() override {
        // Initialize any necessary state or mock resources here
    }

    // Clean up after each test
    void TearDown() override {
        serial.disconnect();  // Disconnect after each test to reset state
    }
};

// Test establishing a serial connection to the Arduino
TEST_F(SerialPortTest, TestConnection) {
    

    bool isConnected = serial.connect(portName, portSpeed);
    EXPECT_TRUE(isConnected);  // Ensure the connection is successful
}

// Test sending a valid command to start the game
TEST_F(SerialPortTest, TestStartGame) {
    

    bool isConnected = serial.connect(portName, portSpeed);
    EXPECT_TRUE(isConnected);

    // Send the StartGame command to the Arduino
    string command = "StartGame";
    string response = serial.sendMessage(command);

    // Очікуємо лише початкове повідомлення "GameStarted", а інші частини (наприклад, дошка) ігноруємо
    string expectedResponse = "GameStarted";
    EXPECT_EQ(normalizeNewlines(response.substr(0, expectedResponse.length())), normalizeNewlines(expectedResponse));
}

// Test setting a valid game mode (e.g., Man vs Man)
TEST_F(SerialPortTest, TestSetGameMode) {
    

    bool isConnected = serial.connect(portName, portSpeed);
    EXPECT_TRUE(isConnected);

    string command = "SetMode 1";  // Man vs Man mode
    string response = serial.sendMessage(command);

    // Очікуємо, що відповідь буде "Mode set to 1", але без зайвих символів
    string expectedResponse = "Mode set to 1";
    EXPECT_EQ(normalizeNewlines(response.substr(0, expectedResponse.length())), normalizeNewlines(expectedResponse));
}

// Test sending an invalid game mode
TEST_F(SerialPortTest, TestInvalidGameMode) {
   

    bool isConnected = serial.connect(portName, portSpeed);
    EXPECT_TRUE(isConnected);

    string command = "SetMode 4";
    string response = serial.sendMessage(command);
    string expectedResponse = "InvailChoice";
    EXPECT_EQ(normalizeNewlines(response.substr(0, expectedResponse.length())), normalizeNewlines(expectedResponse));
}

int main(int argc, char** argv) {
    portName = "COM6";  // за замовчуванням COM3
    portSpeed = 9600;       // за замовчуванням 9600 біт/с

    if (argc > 1) {
        portName = argv[1];
        cout << "Using COM port: " << portName << endl;
    }
    else {
        wcout << L"No COM port specified, using default COM3." << endl;
    }

    if (argc > 2) {
        portSpeed = stoi(argv[2]);
        cout << "Using baud rate: " << portSpeed << endl;
    }
    else {
        cout << "No baud rate specified, using default 9600." << endl;
    }

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}