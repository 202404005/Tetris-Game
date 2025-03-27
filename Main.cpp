You said:
#include <iostream>
 #include <vector>
 #include <conio.h>
 #include <windows.h>
 #include <ctime>
 #include <memory>
 
 using namespace std;
 
 const int WIDTH = 10;
 const int HEIGHT = 20;
 
 // Unicode blocks for better visuals
 const char BLOCK = 219;  // █
 const char EMPTY = ' ';
 
 void setupConsole() {
     SetConsoleOutputCP(65001); // Enable UTF-8 encoding
 }
 
 class Tetromino {
 public:
     vector<vector<int>> shape;
     int x, y;
     int color;
 
     Tetromino(vector<vector<int>> s, int c) : shape(s), x(WIDTH / 2 - 2), y(0), color(c) {}
 
     vector<vector<int>> getRotatedShape() {
         int rows = shape.size();
         int cols = shape[0].size();
         vector<vector<int>> rotated(cols, vector<int>(rows, 0));
 
         for (int i = 0; i < rows; i++)
             for (int j = 0; j < cols; j++)
                 rotated[j][rows - 1 - i] = shape[i][j];
 
         return rotated;
     }
 };
 
 // Updated colors for better UI
 vector<Tetromino> tetrominoes = {
     Tetromino({{1, 1, 1, 1}}, 9),   // I-piece (Cyan)
     Tetromino({{1, 1}, {1, 1}}, 14), // O-piece (Yellow)
     Tetromino({{0, 1, 0}, {1, 1, 1}}, 5), // T-piece (Purple)
     Tetromino({{0, 1, 1}, {1, 1, 0}}, 4), // Z-piece (Red)
     Tetromino({{1, 1, 0}, {0, 1, 1}}, 2), // S-piece (Green)
     Tetromino({{1, 0, 0}, {1, 1, 1}}, 6), // L-piece (Orange)
     Tetromino({{0, 0, 1}, {1, 1, 1}}, 1)  // J-piece (Blue)
 };
 
 class Tetris {
 private:
     vector<vector<int>> grid;
     unique_ptr<Tetromino> currentPiece;
     int score;
     bool gameOver;
     int level;
     int speed;
 
     void setConsoleColor(int color) {
         SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
     }
 
 public:
 Tetris() : score(0), gameOver(false), level(1), speed(300) {  // Reduce initial speed
     grid = vector<vector<int>>(HEIGHT, vector<int>(WIDTH, 0));
     spawnPiece();
 }
 
 
     void spawnPiece() {
         currentPiece = make_unique<Tetromino>(tetrominoes[rand() % tetrominoes.size()]);
         if (checkCollision(currentPiece->x, currentPiece->y, currentPiece->shape))
             gameOver = true;
     }
 
     bool checkCollision(int x, int y, vector<vector<int>> shape) {
         for (int i = 0; i < shape.size(); i++) {
             for (int j = 0; j < shape[i].size(); j++) {
                 if (shape[i][j] && (x + j < 0 || x + j >= WIDTH || y + i >= HEIGHT || grid[y + i][x + j])) {
                     return true;
                 }
             }
         }
         return false;
     }
 
     void mergePiece() {
         for (int i = 0; i < currentPiece->shape.size(); i++) {
             for (int j = 0; j < currentPiece->shape[i].size(); j++) {
                 if (currentPiece->shape[i][j])
                     grid[currentPiece->y + i][currentPiece->x + j] = currentPiece->color;
             }
         }
     }
 
     void clearLines() {
         for (int i = HEIGHT - 1; i >= 0; i--) {
             bool full = true;
             for (int j = 0; j < WIDTH; j++) {
                 if (!grid[i][j]) {
                     full = false;
                     break;
                 }
             }
             if (full) {
                 grid.erase(grid.begin() + i);
                 grid.insert(grid.begin(), vector<int>(WIDTH, 0));
                 score += 100;
                 
                 // Increase level and speed more frequently
                 if (score % 300 == 0) {  
                     speed = max(50, speed - 30);  // Reduce speed faster
                     level++;
                 }
             }
         }
     }
     
     void move(int dx, int dy) {
         if (!checkCollision(currentPiece->x + dx, currentPiece->y + dy, currentPiece->shape)) {
             currentPiece->x += dx;
             currentPiece->y += dy;
         } else if (dy > 0) {
             mergePiece();
             clearLines();
             spawnPiece();
         }
     }
 
     void rotatePiece() {
         vector<vector<int>> rotated = currentPiece->getRotatedShape();
         for (int shift : {-1, 1, -2, 2}) {
             if (!checkCollision(currentPiece->x + shift, currentPiece->y, rotated)) {
                 currentPiece->x += shift;
                 currentPiece->shape = rotated;
                 return;
             }
         }
     }
 
     void drop() {
         while (!checkCollision(currentPiece->x, currentPiece->y + 1, currentPiece->shape))
             currentPiece->y++;
         move(0, 1);
     }
 
     void draw() {
         system("cls");
 
         // Display Score and Controls
         cout << "TETRIS\nScore: " << score << "  Level: " << level << "\n";
         cout << "Controls: ← Move | → Move | ↑ Rotate | ↓ Soft Drop | Space: Hard Drop | ESC: Exit\n\n";
 
         // Draw the border
         cout << "┌";
         for (int i = 0; i < WIDTH * 2; i++) cout << "─";
         cout << "┐\n";
 
         vector<vector<int>> display = grid;
         for (int i = 0; i < currentPiece->shape.size(); i++) {
             for (int j = 0; j < currentPiece->shape[i].size(); j++) {
                 if (currentPiece->shape[i][j])
                     display[currentPiece->y + i][currentPiece->x + j] = currentPiece->color;
             }
         }
 
         // Print the grid with border
         for (const auto& row : display) {
             cout << "│";
             for (int cell : row) {
                 if (cell) {
                     setConsoleColor(cell);
                     cout << "██";  // Blocks
                     setConsoleColor(7);
                 } else {
                     cout << "  ";  // Empty space
                 }
             }
             cout << "│\n";
         }
 
         cout << "└";
         for (int i = 0; i < WIDTH * 2; i++) cout << "─";
         cout << "┘\n";
     }
 
     void run() {
         while (!gameOver) {
             draw();
             for (int i = 0; i < speed / 100; i++) {  // Increase update frequency
                 if (_kbhit()) {
                     char ch = _getch();
                     switch (ch) {
                         case 75: move(-1, 0); break;  
                         case 77: move(1, 0); break;   
                         case 72: rotatePiece(); break; 
                         case 80: move(0, 1); break;   
                         case 32: drop(); break;       
                         case 27: gameOver = true; break;
                     }
                 }
                 Sleep(30);  // Reduce sleep delay
             }
             
             move(0, 1);
         }
         cout << "Game Over! Final Score: " << score << endl;
     }
 };
 
 int main() {
     srand(time(0));
     setupConsole();
     Tetris game;
     game.run();               
     return 0;
 }
