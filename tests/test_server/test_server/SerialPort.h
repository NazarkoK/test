/**
 * @file SerialPort.h
 * @brief Header file for serial port communication.
 */

#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <fstream>
#include <string>
#include <windows.h>

 /** @brief Port name for the serial connection. */
extern std::string port;

/** @brief Baud rate for the serial connection. */
extern int portSpeed;

/**
 * @class SerialCommunication
 * @brief Manages serial communication.
 */
class SerialCommunication {
private:
    HANDLE hSerial = INVALID_HANDLE_VALUE; /**< Handle to the serial port. */

public:
    /**
     * @brief Establishes a connection to the serial port.
     * @param port Port name (e.g., "COM3").
     * @param baudRate Communication baud rate (e.g., 9600).
     * @return True if the connection is successful, false otherwise.
     */
    bool connect(const std::string& port, int baudRate);

    /**
     * @brief Sends a message through the serial port.
     * @param message The message to send.
     * @return The response received from the serial port.
     */
    std::string sendMessage(const std::string& message);

    /**
     * @brief Closes the serial port connection.
     */
    void disconnect();

    /**
     * @brief Displays a tic-tac-toe board based on the given state.
     * @param boardState A string representing the board's state (e.g., "XOXOXOXOX").
     */
    void drawBoard(const std::string& boardState);
};

/**
 * @brief Loads configuration settings from a file.
 * @param filename Path to the configuration file.
 */
void loadConfig(const std::string& filename);

#endif // SERIALPORT_H
