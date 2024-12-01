  /**
  * @file server.ino
  * @brief Arduino implementation of a Tic-Tac-Toe game server with multiple modes.
  */

  #include <Arduino.h>

  /** @brief Size of the Tic-Tac-Toe board. */
  const int BOARD_SIZE = 3;

  /** @brief Symbol representing player X. */
  const char PLAYER_X = 'X';

  /** @brief Symbol representing player O. */
  const char PLAYER_O = 'O';

  /** @brief Game mode: Man vs Man. */
  const int MODE_MAN_VS_MAN = 1;

  /** @brief Game mode: Man vs AI. */
  const int MODE_MAN_VS_AI = 2;

  /** @brief Game mode: AI vs AI. */
  const int MODE_AI_VS_AI = 3;

  /** @brief The game board. */
  char board[BOARD_SIZE][BOARD_SIZE] = { { '1', '2', '3' }, { '4', '5', '6' }, { '7', '8', '9' } };

  /** @brief Indicates whether the game has started. */
  bool isGameStarted = false;

  /** @brief Current game mode. */
  int gameMode = 0;

  /** @brief Last move made by the server (for AI games). */
  int lastServerMove = -1;

  /** @brief Number of moves made in the current game. */
  int playerCount = 0;

  /** @brief Current player in AI vs AI mode. */
  char globalCurrentPlayer = PLAYER_X;

  /**
  * @brief Initializes the Arduino.
  */
  void setup() {
      Serial.begin(9600);
  }

  /**
  * @brief Main game loop.
  */
  void loop() {
      if (Serial.available() > 0) {
          String command = Serial.readStringUntil('\n');

          if (command == "StartGame") {
              startGame();
          } else if (command.startsWith("SetMode ")) {
              setGameMode(command);
          }

          if (gameMode == MODE_MAN_VS_MAN) {
              handleManvsMan(command);
          }
          if (gameMode == MODE_MAN_VS_AI) {
              handleManvsAI(command);
          }
          if (gameMode == MODE_AI_VS_AI) {
              handleAIvsAI(command);
          }
      }
  }

  /**
  * @brief Starts a new game and resets the board.
  */
  void startGame() {
      resetBoard();
      isGameStarted = true;
      Serial.println("GameStarted");
      printBoardGraphically();
  }

  /**
  * @brief Sets the game mode based on the input command.
  * @param command The received command containing the mode.
  */
  void setGameMode(const String& command) {
      String mode = command.substring(8);
      gameMode = mode.toInt();
      if (gameMode < 1 || gameMode > 3) {
          Serial.println("InvailChoice");
          return;
      }

      Serial.println("Mode set to " + mode);
  }

  /**
  * @brief Handles moves in Man vs Man mode.
  * @param command The received move command.
  */
  void handleManvsMan(String command) {
      if (command.startsWith("Move") && isGameStarted) {
          int position = command.substring(5).toInt();

          char player = (playerCount % 2 == 0) ? PLAYER_X : PLAYER_O;

          if (makePlayerMove(position, player)) {
              printBoardGraphically();
              checkGameStatus();
              playerCount++;
          } else {
              Serial.println("InvalidMove");
          }
      }
  }

  /**
  * @brief Handles moves in Man vs AI mode.
  * @param command The received move command.
  */
  void handleManvsAI(String command) {
      if (command.startsWith("Move") && isGameStarted) {
          int position = command.substring(5).toInt();

          if (makePlayerMove(position, PLAYER_X)) {
              if (!checkGameStatus()) {
                  int aiMove[2];
                  bestMove(PLAYER_O, aiMove);
                  makeAIMove(aiMove, PLAYER_O);
                  printBoardGraphically();
                  checkGameStatus();
              }
          } else {
              Serial.println("InvalidMove");
          }
      }
  }

  /**
  * @brief Handles moves in AI vs AI mode.
  * @param command The received command (unused in this mode).
  */
  void handleAIvsAI(String command) {
      if (isGameStarted) {
          globalCurrentPlayer = PLAYER_X;

          while (!checkGameStatus()) {
              int aiMove[2];
              bestMove(globalCurrentPlayer, aiMove);
              makeAIMove(aiMove, globalCurrentPlayer);
              printBoardGraphically();

              delay(500);
              if (checkGameStatus()) break;

              globalCurrentPlayer = opponent(globalCurrentPlayer);
          }
      }
  }

  /**
  * @brief Makes a move for a player.
  * @param position The board position to mark.
  * @param player The current player symbol.
  * @return True if the move is valid, false otherwise.
  */
  bool makePlayerMove(int position, char player) {
      if (isPositionValid(position)) {
          int row = (position - 1) / BOARD_SIZE;
          int col = (position - 1) % BOARD_SIZE;
          board[row][col] = player;
          return true;
      }
      return false;
  }

  /**
  * @brief Makes a move for the AI.
  * @param aiMove The calculated AI move as row and column indices.
  * @param player The AI player symbol.
  */
  void makeAIMove(int aiMove[2], char player) {
      board[aiMove[0]][aiMove[1]] = player;
      lastServerMove = aiMove[0] * BOARD_SIZE + aiMove[1] + 1;
      Serial.println("ServerMove: " + String(lastServerMove));
  }

  /**
  * @brief Checks if a board position is valid.
  * @param position The board position to check.
  * @return True if the position is valid, false otherwise.
  */
  bool isPositionValid(int position) {
      int row = (position - 1) / BOARD_SIZE;
      int col = (position - 1) % BOARD_SIZE;
      return (position >= 1 && position <= 9 && board[row][col] != PLAYER_X && board[row][col] != PLAYER_O);
  }

  /**
  * @brief Checks the status of the game.
  * @return True if the game has ended, false otherwise.
  */
  bool checkGameStatus() {
      if (checkWin(PLAYER_X)) {
          Serial.println("X Wins");
          return true;
      } else if (checkWin(PLAYER_O)) {
          Serial.println("O Wins");
          return true;
      } else if (isBoardFull()) {
          Serial.println("Draw");
          return true;
      }
      return false;
  }

  /**
  * @brief Resets the game board to its initial state.
  */
  void resetBoard() {
      for (int i = 0; i < BOARD_SIZE; i++) {
          for (int j = 0; j < BOARD_SIZE; j++) {
              board[i][j] = '1' + (i * BOARD_SIZE + j);
          }
      }
      lastServerMove = -1;
  }

  /**
  * @brief Checks if a player has won the game.
  * @param player The player symbol to check for a win.
  * @return True if the player has won, false otherwise.
  */
  bool checkWin(char player) {
      for (int i = 0; i < BOARD_SIZE; i++) {
          // Check rows and columns
          if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
              (board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
              return true;
          }
      }
      // Check diagonals
      return (board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
            (board[0][2] == player && board[1][1] == player && board[2][0] == player);
  }

  /**
  * @brief Checks if the game board is full.
  * @return True if the board is full, false otherwise.
  */
  bool isBoardFull() {
      for (int i = 0; i < BOARD_SIZE; i++) {
          for (int j = 0; j < BOARD_SIZE; j++) {
              if (board[i][j] != PLAYER_X && board[i][j] != PLAYER_O) {
                  return false;
              }
          }
      }
      return true;
  }

  /**
  * @brief Prints the current state of the board in a graphical format to the Serial monitor.
  */
  void printBoardGraphically() {
      Serial.println("-------------");
      for (int i = 0; i < BOARD_SIZE; i++) {
          Serial.print("| ");
          for (int j = 0; j < BOARD_SIZE; j++) {
              Serial.print(board[i][j]);
              Serial.print(" | ");
          }
          Serial.println();
          Serial.println("-------------");
      }
      Serial.println();
  }

  /**
  * @brief Gets the opponent's symbol for a given player.
  * @param player The player symbol ('X' or 'O').
  * @return The opponent's symbol.
  */
  char opponent(char player) {
      return (player == PLAYER_X) ? PLAYER_O : PLAYER_X;
  }

  /**
  * @brief Minimax algorithm to determine the best score for a given player.
  * @param currentPlayer The current player being evaluated.
  * @param aiPlayer The AI player symbol.
  * @param depth The depth of the recursive search.
  * @return The best score for the current player.
  */
  int minimax(char currentPlayer, char aiPlayer, int depth) {
      if (checkWin(aiPlayer)) {
          return 10 - depth; // Favorable outcome for AI
      } else if (checkWin(opponent(aiPlayer))) {
          return depth - 10; // Unfavorable outcome for AI
      } else if (isBoardFull()) {
          return 0; // Draw
      }

      int bestScore = (currentPlayer == aiPlayer) ? -1000 : 1000;

      for (int i = 0; i < BOARD_SIZE; i++) {
          for (int j = 0; j < BOARD_SIZE; j++) {
              if (board[i][j] != PLAYER_X && board[i][j] != PLAYER_O) {
                  // Simulate the move
                  board[i][j] = currentPlayer;
                  int score = minimax(opponent(currentPlayer), aiPlayer, depth + 1);
                  // Undo the move
                  board[i][j] = '1' + (i * BOARD_SIZE + j);
                  if (currentPlayer == aiPlayer) {
                      bestScore = max(bestScore, score);
                  } else {
                      bestScore = min(bestScore, score);
                  }
              }
          }
      }
      return bestScore;
  }

  /**
  * @brief Finds the best move for the AI using the Minimax algorithm.
  * @param aiPlayer The AI player symbol.
  * @param move An array to store the row and column indices of the best move.
  */
  void bestMove(char aiPlayer, int move[2]) {
      int bestScore = -1000;
      move[0] = -1;
      move[1] = -1;

      for (int i = 0; i < BOARD_SIZE; i++) {
          for (int j = 0; j < BOARD_SIZE; j++) {
              if (board[i][j] != PLAYER_X && board[i][j] != PLAYER_O) {
                  // Simulate the move
                  board[i][j] = aiPlayer;
                  int score = minimax(opponent(aiPlayer), aiPlayer, 0);
                  // Undo the move
                  board[i][j] = '1' + (i * BOARD_SIZE + j);
                  if (score > bestScore) {
                      bestScore = score;
                      move[0] = i;
                      move[1] = j;
                  }
              }
          }
      }
  }
