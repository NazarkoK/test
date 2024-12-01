#include <gtest/gtest.h>
#include "SerialPort.h"
#include <string>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

using namespace std;
using json = nlohmann::json;

string portName; ///< ����� �������� ����� ��� ������������.
int portSpeed; ///< �������� ����� (Baud rate).

/**
 * @brief �������� ������� ������ ����� � �����.
 */
string normalizeNewlines(const string& str) {
    string normalized = str;
    replace(normalized.begin(), normalized.end(), '\r', '\n');
    return normalized;
}

/**
 * @brief �������� ���� ��� ���������� ��������������� �������� �����.
 */
class SerialPortTest : public ::testing::Test {
protected:
    SerialCommunication serial; ///< ��'��� ��� ���������� �������� �����.

    void SetUp() override {}

    void TearDown() override {
        serial.disconnect();
    }
};

TEST_F(SerialPortTest, TestDrawBoard) {
    string boardState = "X O   O X   O";  // ��������� ���� � �������� ���������
    stringstream output;
    streambuf* orig_buf = cout.rdbuf(output.rdbuf()); // ϳ�������� �������� ���� ���������.

    serial.drawBoard(boardState); // ��������� ������� ��� ��������� �����.

    string expectedOutput = "-------------\n| X |   | O | \n-------------\n|   |   |   | \n-------------\n| O |   | X | \n-------------\n";
    EXPECT_EQ(normalizeNewlines(output.str()), normalizeNewlines(expectedOutput)); // ���������� ���������� ���������.

    cout.rdbuf(orig_buf);  // ³��������� ����������� ����.
}



/**
 * @brief ����� ������� loadConfig.
 *
 * ��� ���� �������� ��������� ������������ ����������� � JSON �����.
 */
TEST_F(SerialPortTest, TestLoadConfig) {
    stringstream configStream;
    configStream << R"({
        "Connection": {
            "port": "COM3",
            "baudRate": 9600
        }
    })";
    istringstream input(configStream.str());

    json j;
    input >> j;

    // �������, �� ������������ �������������� ���������.
    EXPECT_EQ(j["Connection"]["port"].get<string>(), "COM3");
    EXPECT_EQ(j["Connection"]["baudRate"].get<int>(), 9600);
}

/**
 * @brief ����� �'������� ����� ������� ����.
 */
TEST_F(SerialPortTest, TestConnection) {
    bool isConnected = serial.connect(portName, portSpeed);
    EXPECT_TRUE(isConnected);  ///< ����������, �� �'������� ������.
}

/**
 * @brief ����� ���������� ������� "StartGame".
 */
TEST_F(SerialPortTest, TestStartGame) {
    bool isConnected = serial.connect(portName, portSpeed);
    EXPECT_TRUE(isConnected);

    string command = "StartGame";
    string response = serial.sendMessage(command);

    string expectedResponse = "GameStarted";
    EXPECT_EQ(normalizeNewlines(response.substr(0, expectedResponse.length())), normalizeNewlines(expectedResponse));
}

/**
 * @brief ����� ���������� ������� "SetMode 1".
 */
TEST_F(SerialPortTest, TestSetGameMode) {
    bool isConnected = serial.connect(portName, portSpeed);
    EXPECT_TRUE(isConnected);

    string command = "SetMode 1";
    string response = serial.sendMessage(command);

    string expectedResponse = "Mode set to 1";
    EXPECT_EQ(normalizeNewlines(response.substr(0, expectedResponse.length())), normalizeNewlines(expectedResponse));
}

/**
 * @brief ����� ���������� �������� ������� "SetMode 4".
 */
TEST_F(SerialPortTest, TestInvalidGameMode) {
    bool isConnected = serial.connect(portName, portSpeed);
    EXPECT_TRUE(isConnected);

    string command = "SetMode 4";
    string response = serial.sendMessage(command);
    string expectedResponse = "InvailChoice";
    EXPECT_EQ(normalizeNewlines(response.substr(0, expectedResponse.length())), normalizeNewlines(expectedResponse));
}

/**
 * @brief ������� ������� ��� ������� �����.
 */
int main(int argc, char** argv) {
    portName = "COM6"; ///< �� ������������� COM ����.
    portSpeed = 9600; ///< �� ������������� �������� �����.

    if (argc > 1) {
        portName = argv[1];
        cout << "Using COM port: " << portName << endl;
    }

    if (argc > 2) {
        portSpeed = stoi(argv[2]);
        cout << "Using baud rate: " << portSpeed << endl;
    }

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS(); ///< ������� �� �����.
}
