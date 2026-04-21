#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <iostream>

/*
 * You may need to define some global variables for the information of the game map here.
 * Although we don't encourage to use global variables in real cpp projects, you may have to use them because the use of
 * class is not taught yet. However, if you are member of A-class or have learnt the use of cpp class, member functions,
 * etc., you're free to modify this structure.
 */
int rows;         // The count of rows of the game map. You MUST NOT modify its name.
int columns;      // The count of columns of the game map. You MUST NOT modify its name.
int total_mines;  // The count of mines of the game map. You MUST NOT modify its name. You should initialize this
                  // variable in function InitMap. It will be used in the advanced task.
int game_state;  // The state of the game, 0 for continuing, 1 for winning, -1 for losing. You MUST NOT modify its name.

// Global variables for game state
const int MAX_SIZE = 30;
bool is_mine[MAX_SIZE][MAX_SIZE];       // true if the block is a mine
bool visited[MAX_SIZE][MAX_SIZE];       // true if the block has been visited
bool marked[MAX_SIZE][MAX_SIZE];       // true if the block has been marked
int mine_count[MAX_SIZE][MAX_SIZE];    // number of adjacent mines for each block
int visit_count = 0;                    // number of visited non-mine blocks
int marked_mine_count = 0;              // number of correctly marked mines

// Direction arrays for 8 neighbors
const int dr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
const int dc[] = {-1, 0, 1, -1, 1, -1, 0, 1};

/**
 * @brief The definition of function InitMap()
 *
 * @details This function is designed to read the initial map from stdin. For example, if there is a 3 * 3 map in which
 * mines are located at (0, 1) and (1, 2) (0-based), the stdin would be
 *     3 3
 *     .X.
 *     ...
 *     ..X
 * where X stands for a mine block and . stands for a normal block. After executing this function, your game map
 * would be initialized, with all the blocks unvisited.
 */
void InitMap() {
  std::cin >> rows >> columns;
  total_mines = 0;
  game_state = 0;
  visit_count = 0;
  marked_mine_count = 0;

  // Initialize arrays
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      visited[i][j] = false;
      marked[i][j] = false;
      is_mine[i][j] = false;
      mine_count[i][j] = 0;
    }
  }

  // Read the map
  for (int i = 0; i < rows; i++) {
    std::string line;
    std::cin >> line;
    for (int j = 0; j < columns; j++) {
      if (line[j] == 'X') {
        is_mine[i][j] = true;
        total_mines++;
      }
    }
  }

  // Calculate mine count for each non-mine block
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (!is_mine[i][j]) {
        for (int d = 0; d < 8; d++) {
          int ni = i + dr[d];
          int nj = j + dc[d];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns && is_mine[ni][nj]) {
            mine_count[i][j]++;
          }
        }
      }
    }
  }
}

/**
 * @brief Recursive function to visit blocks with 0 adjacent mines
 */
void dfs_visit(int r, int c) {
  if (r < 0 || r >= rows || c < 0 || c >= columns) return;
  if (visited[r][c] || marked[r][c]) return;
  if (is_mine[r][c]) return;

  visited[r][c] = true;
  visit_count++;

  if (mine_count[r][c] == 0) {
    for (int d = 0; d < 8; d++) {
      dfs_visit(r + dr[d], c + dc[d]);
    }
  }
}

/**
 * @brief The definition of function VisitBlock(int, int)
 *
 * @details This function is designed to visit a block in the game map. We take the 3 * 3 game map above as an example.
 * At the beginning, if you call VisitBlock(0, 0), the return value would be 0 (game continues), and the game map would
 * be
 *     1??
 *     ???
 *     ???
 * If you call VisitBlock(0, 1) after that, the return value would be -1 (game ends and the players loses) , and the
 * game map would be
 *     1X?
 *     ???
 *     ???
 * If you call VisitBlock(0, 2), VisitBlock(2, 0), VisitBlock(1, 2) instead, the return value of the last operation
 * would be 1 (game ends and the player wins), and the game map would be
 *     1@1
 *     122
 *     01@
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 *
 * @note You should edit the value of game_state in this function. Precisely, edit it to
 *    0  if the game continues after visit that block, or that block has already been visited before.
 *    1  if the game ends and the player wins.
 *    -1 if the game ends and the player loses.
 *
 * @note For invalid operation, you should not do anything.
 */
void VisitBlock(int r, int c) {
  if (r < 0 || r >= rows || c < 0 || c >= columns) return;
  if (visited[r][c] || marked[r][c]) return;

  if (is_mine[r][c]) {
    game_state = -1;
    visited[r][c] = true;
    return;
  }

  dfs_visit(r, c);

  // Check for win condition
  int non_mine_count = rows * columns - total_mines;
  if (visit_count == non_mine_count) {
    game_state = 1;
  }
}

/**
 * @brief The definition of function MarkMine(int, int)
 *
 * @details This function is designed to mark a mine in the game map.
 * If the block being marked is a mine, show it as "@".
 * If the block being marked isn't a mine, END THE GAME immediately. (NOTE: This is not the same rule as the real
 * game) And you don't need to
 *
 * For example, if we use the same map as before, and the current state is:
 *     1?1
 *     ???
 *     ???
 * If you call MarkMine(0, 1), you marked the right mine. Then the resulting game map is:
 *     1@1
 *     ???
 *     ???
 * If you call MarkMine(1, 0), you marked the wrong mine(There's no mine in grid (1, 0)).
 * The game_state would be -1 and game ends immediately. The game map would be:
 *     1??
 *     X??
 *     ???
 * This is different from the Minesweeper you've played. You should beware of that.
 *
 * @param r The row coordinate (0-based) of the block to be marked.
 * @param c The column coordinate (0-based) of the block to be marked.
 *
 * @note You should edit the value of game_state in this function. Precisely, edit it to
 *    0  if the game continues after visit that block, or that block has already been visited before.
 *    1  if the game ends and the player wins.
 *    -1 if the game ends and the player loses.
 *
 * @note For invalid operation, you should not do anything.
 */
void MarkMine(int r, int c) {
  if (r < 0 || r >= rows || c < 0 || c >= columns) return;
  if (visited[r][c] || marked[r][c]) return;

  if (!is_mine[r][c]) {
    game_state = -1;
    return;
  }

  marked[r][c] = true;
  marked_mine_count++;

  // Check for win condition
  int non_mine_count = rows * columns - total_mines;
  if (visit_count == non_mine_count) {
    game_state = 1;
  }
}

/**
 * @brief The definition of function AutoExplore(int, int)
 *
 * @details This function is designed to auto-visit adjacent blocks of a certain block.
 * See README.md for more information
 *
 * For example, if we use the same map as before, and the current map is:
 *     ?@?
 *     ?2?
 *     ??@
 * Then auto explore is available only for block (1, 1). If you call AutoExplore(1, 1), the resulting map will be:
 *     1@1
 *     122
 *     01@
 * And the game ends (and player wins).
 */
void AutoExplore(int r, int c) {
  if (r < 0 || r >= rows || c < 0 || c >= columns) return;
  if (!visited[r][c] || is_mine[r][c]) return;

  // Count marked mines around (r, c)
  int marked_around = 0;
  for (int d = 0; d < 8; d++) {
    int ni = r + dr[d];
    int nj = c + dc[d];
    if (ni >= 0 && ni < rows && nj >= 0 && nj < columns && marked[ni][nj]) {
      marked_around++;
    }
  }

  // Only auto-explore if marked mines equal the mine count
  if (marked_around != mine_count[r][c]) return;

  // Visit all non-mine, unmarked, unvisited neighbors
  for (int d = 0; d < 8; d++) {
    int ni = r + dr[d];
    int nj = c + dc[d];
    if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
      if (!visited[ni][nj] && !marked[ni][nj]) {
        if (is_mine[ni][nj]) {
          game_state = -1;
          return;
        }
        dfs_visit(ni, nj);
      }
    }
  }

  // Check for win condition
  int non_mine_count = rows * columns - total_mines;
  if (visit_count == non_mine_count) {
    game_state = 1;
  }
}

/**
 * @brief The definition of function ExitGame()
 *
 * @details This function is designed to exit the game.
 * It outputs a line according to the result, and a line of two integers, visit_count and marked_mine_count,
 * representing the number of blocks visited and the number of marked mines taken respectively.
 *
 * @note If the player wins, we consider that ALL mines are correctly marked.
 */
void ExitGame() {
  if (game_state == 1) {
    std::cout << "YOU WIN!" << std::endl;
    std::cout << visit_count << " " << total_mines << std::endl;
  } else {
    std::cout << "GAME OVER!" << std::endl;
    std::cout << visit_count << " " << marked_mine_count << std::endl;
  }
  exit(0);  // Exit the game immediately
}

/**
 * @brief The definition of function PrintMap()
 *
 * @details This function is designed to print the game map to stdout. We take the 3 * 3 game map above as an example.
 * At the beginning, if you call PrintMap(), the stdout would be
 *    ???
 *    ???
 *    ???
 * If you call VisitBlock(2, 0) and PrintMap() after that, the stdout would be
 *    ???
 *    12?
 *    01?
 * If you call VisitBlock(0, 1) and PrintMap() after that, the stdout would be
 *    ?X?
 *    12?
 *    01?
 * If the player visits all blocks without mine and call PrintMap() after that, the stdout would be
 *    1@1
 *    122
 *    01@
 * (You may find the global variable game_state useful when implementing this function.)
 *
 * @note Use std::cout to print the game map, especially when you want to try the advanced task!!!
 */
void PrintMap() {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (visited[i][j]) {
        if (is_mine[i][j]) {
          std::cout << 'X';
        } else {
          std::cout << mine_count[i][j];
        }
      } else if (marked[i][j]) {
        if (is_mine[i][j]) {
          if (game_state == 1) {
            // Victory: show all mines as @
            std::cout << '@';
          } else {
            std::cout << '@';
          }
        } else {
          // Marked non-mine - this should only happen on game over
          std::cout << 'X';
        }
      } else {
        // Unvisited
        if (game_state == 1 && is_mine[i][j]) {
          // Victory: show all mines as @
          std::cout << '@';
        } else {
          std::cout << '?';
        }
      }
    }
    std::cout << std::endl;
  }
}

#endif
