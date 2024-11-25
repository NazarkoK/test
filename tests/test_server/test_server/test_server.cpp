#include <gtest/gtest.h>
#include "SerialPort.h"
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;
string portName;
int portSpeed;

string normalizeNewlines(const string& str) {
    string normalized = str;
    replace(normalized.begin(), normalized.end(), '\r', '\n');
    return normalized;
}

class SerialPortTest : public ::testing::Test {
protected:
    SerialCommunication serial;

    void SetUp() override {}
    void TearDown() override {
        serial.disconnect();
    }
};

TEST_F(SerialPortTest, TestConnection) {
    

    bool isConnected = serial.connect(portName, portSpeed);
    EXPECT_TRUE(isConnected);
}

TEST_F(SerialPortTest, TestStartGame) {
    

    bool isConnected = serial.connect(portName, portSpeed);
    EXPECT_TRUE(isConnected);

    string command = "StartGame";
    string response = serial.sendMessage(command);

    string expectedResponse = "GameStarted";
    EXPECT_EQ(normalizeNewlines(response.substr(0, expectedResponse.length())), normalizeNewlines(expectedResponse));
}

TEST_F(SerialPortTest, TestSetGameMode) {
    

    bool isConnected = serial.connect(portName, portSpeed);
    EXPECT_TRUE(isConnected);

    string command = "SetMode 1";
    string response = serial.sendMessage(command);

    string expectedResponse = "Mode set to 1";
    EXPECT_EQ(normalizeNewlines(response.substr(0, expectedResponse.length())), normalizeNewlines(expectedResponse));
}

TEST_F(SerialPortTest, TestInvalidGameMode) {
   

    bool isConnected = serial.connect(portName, portSpeed);
    EXPECT_TRUE(isConnected);

    string command = "SetMode 4";
    string response = serial.sendMessage(command);
    string expectedResponse = "InvailChoice";
    EXPECT_EQ(normalizeNewlines(response.substr(0, expectedResponse.length())), normalizeNewlines(expectedResponse));
}

int main(int argc, char** argv) {
    portName = "COM6";
    portSpeed = 9600;

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