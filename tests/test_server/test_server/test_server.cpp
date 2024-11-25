/**
 * @file test_server.cpp
 * @brief Test suite for serial communication using the SerialPort class.
 *
 * This file contains test cases for testing the serial communication functionality,
 * including connection to the serial port, sending commands, and receiving responses.
 * The tests are written using Google Test framework (gtest).
 * It covers:
 * - Establishing a connection with the serial port.
 * - Sending a "StartGame" command and validating the response.
 * - Sending a "SetMode" command and validating the game mode response.
 * - Handling an invalid "SetMode" command.
 */

#include <gtest/gtest.h>
#include "SerialPort.h"
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

string portName; ///< Name of the serial port to be used.
int portSpeed; ///< Baud rate for serial communication.

/**
 * @brief Normalizes newline characters in the input string.
 *
 * This function replaces all occurrences of carriage return '\r' with newline '\n' characters
 * to ensure consistency in the handling of line breaks across different platforms.
 *
 * @param str The input string to be normalized.
 * @return A string with normalized newlines.
 */
string normalizeNewlines(const string& str) {
    string normalized = str;
    replace(normalized.begin(), normalized.end(), '\r', '\n');
    return normalized;
}

/**
 * @brief Test fixture for serial communication tests.
 *
 * This class provides setup and teardown methods for testing serial communication functionality.
 * The tests are designed to verify the behavior of the SerialCommunication class.
 */
class SerialPortTest : public ::testing::Test {
protected:
    SerialCommunication serial; ///< Instance of the SerialCommunication class for testing.

    /**
     * @brief Sets up the test environment.
     *
     * This method is called before each test. It can be used to initialize resources.
     * In this case, it is empty as there is no need for specific setup.
     */
    void SetUp() override {}

    /**
     * @brief Cleans up after each test.
     *
     * This method is called after each test. It disconnects the serial communication.
     */
    void TearDown() override {
        serial.disconnect();
    }
};

/**
 * @brief Tests the connection to the serial port.
 *
 * This test verifies that the connection to the serial port is successfully established.
 */
TEST_F(SerialPortTest, TestConnection) {
    bool isConnected = serial.connect(portName, portSpeed);
    EXPECT_TRUE(isConnected); ///< Expect the connection to be successful.
}

/**
 * @brief Tests sending the "StartGame" command over the serial connection.
 *
 * This test sends the "StartGame" command to the serial port and verifies that the response
 * matches the expected value "GameStarted".
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
 * @brief Tests sending the "SetMode 1" command over the serial connection.
 *
 * This test sends the "SetMode 1" command to the serial port and verifies that the response
 * matches the expected value "Mode set to 1".
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
 * @brief Tests sending an invalid game mode command.
 *
 * This test sends an invalid "SetMode 4" command to the serial port and verifies that
 * the response matches the expected error message "InvailChoice".
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
 * @brief The main function for running the tests.
 *
 * This function initializes the Google Test framework, parses command-line arguments
 * for serial port name and speed, and runs all tests.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return The result of running the tests (0 for success, non-zero for failure).
 */
int main(int argc, char** argv) {
    portName = "COM6"; ///< Default COM port.
    portSpeed = 9600; ///< Default baud rate.

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
    return RUN_ALL_TESTS(); ///< Run all tests and return the result.
}
