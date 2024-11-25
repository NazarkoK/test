/**
 * @file serialport.h
 * @brief Header file for serial communication functionality.
 */

#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <fstream>
#include <string>
#include <windows.h>

 /**
  * @brief Global port identifier for serial communication.
  */
extern std::string port;

/**
 * @brief Global baud rate for serial communication.
 */
extern int baudRate;

/**
 * @brief Handles serial communication with a device.
 */
class SerialCommunication {
private:
    /**
     * @brief Handle for the serial port.
     */
    HANDLE hSerial = INVALID_HANDLE_VALUE;

public:
    /**
     * @brief Connects to a serial port with the specified settings.
     * @param port The name of the serial port to connect to.
     * @param baudRate The baud rate for the connection.
     * @return True if the connection is successful, false otherwise.
     */
    bool connect(const std::string& port, int baudRate);

    /**
     * @brief Sends a message to the serial port and reads the response.
     * @param message The message to send.
     * @return The response from the serial device.
     */
    std::string sendMessage(const std::string& message);

    /**
     * @brief Disconnects from the serial port.
     */
    void disconnect();

    /**
     * @brief Draws a tic-tac-toe board based on the given board state.
     * @param boardState The state of the board as a string (e.g., "XO XO XO ").
     */
    void drawBoard(const std::string& boardState);
};

/**
 * @brief Loads the configuration settings from a JSON file.
 * @param filename The path to the configuration file.
 */
void loadConfig(const std::string& filename);

#endif // SERIALPORT_H
