# a. detalis about repo
  This repository was created to host the laboratory works of Kachur Nazar, a student of KI-405 group, from the CSAD course. In the future, it will contain ready-made laboratory works from the initial stages to the work program, which will be the end of the course.
	
# b. task detalis
  The goal of the project is to develop a digital version of the classic and well-known Tic-Tac-Toe (3x3) game. The result is an interactive and intuitive platform where two players can compete by placing their symbols (X or O) on a 3x3 grid.
	
# c. student number and detalis
  Since I am the 11th student in the group, my assignment will look like this:
  | Student number  | Student       | Game            | Config format |
  | ----------------| --------------|-----------------|---------------|
  | 11              | Kachur Nazar  | Tik-Tac-Toe 3x3 | JSON          |
	
# d. detalis about technology, program language and HW
  In this project, I am going to use a client-server architecture. The server, which will provide data to the client program, will be represented by an Arduino board (I will announce the specific model later). The Arduion board will perform calculations and return the results to the client program at the end. The language for the project development will be Java or C++/C# (no final decision has been made at this time). Note that there may be changes during project development.
  
# e. details how to install and run the project:
### Repository:
To download this repository you must:  
Clone the repository:  
git clone https://github.com/NazarkoK/csad2425KI405KachurNazar11 

### Client SW:
To build and run the client application, follow these steps:
1. Open the project (`client.sln`) in Microsoft Visual Studio 2019:
2. Build the project:
   - Click on *Build* in the top menu, then select *Build Solution*.
3. Run the application:
   - Click on *Debug* in the top menu, then select *Start Debugging*.

### Server HW:
To build and run the server application, follow these steps:
1. Open the project (`server.ino`) in Arduino IDE:
   - Launch Adrduino IDE;
2. Configure the board:
   - Select the appropriate board from the menu: *Tools* > *Board*;
   - Make sure the correct port is selected: *Tools* > *Port*.
3. Upload the code:
   - Click the *Upload* button to upload the compiled code to your Arduino board.
   - Wait for the upload process to complete. The IDE will display a message when it is finished.

# f. details how to test client sw and server hw:   
## Test Client SW:
To build and run the testing application, follow these steps:
1. Open the project (`csad2425KI405KachurNazar11/tests/test_server/test_server.sln`) in Microsoft Visual Studio 2019:
2. Build the project:
   - Click on Build in the top menu, then select Build Solution.
3. Run the application:
   - Click on Debug in the top menu, then select Start Debugging.
   - If everything is set up correctly, the application should launch, and you can start using it.

## Testing Server and Automatic Flashing Of the Board:  
To build and run the testing application, follow: run the script (`csad2425KI405KachurNazar11/ci/server-script.ps1`) in PowerShell.  
This script performs the following steps:
1. Install Arduino CLI: 
   - The script will check if Arduino CLI is present. If not, it will download and install it.
2. Configure Arduino CLI: 
   - Initializes Arduino CLI configuration, updates the core index, and installs the necessary board.
3. Compile Arduino Sketch: 
   - Compiles the Arduino sketch located in (`src/server/server.ino`) for an Arduino Uno and outputs the .hex file.
4. Upload Compiled Sketch to Arduino: 
   - Uploads the compiled .hex file to the Arduino via the specified COM-port.
5. Build Hardware Testing Project: 
   - Uses MSBuild to compile the hardware testing project from the solution file (`tests/test_server/test_server.sln`).
6. Run Hardware Tests: 
   - Executes the compiled hardware test executable (`test_server.exe`) with parameters specifying the COM-port and generating XML test results.
7. Transform Test Results: 
   - Transforms the generated XML test results into an HTML report using an XSLT stylesheet.
8. Output Test Results: 
   - The final test results are saved in HTML format and are available at the specified output path.

# g. details how to generate dpxygen-documentation:   

To build and generate documentation for the project, follow these steps:  
Run the script (`csad2425KI405KachurNazar11/ci/documentation-script.ps1`) in PowerShell.  

The generated documentation can be found in the following directories:
- HTML: (`ci/documentation/html`)
- XML: (`ci/documentation/xml`)
- LaTeX: (`ci/documentation/latex`)