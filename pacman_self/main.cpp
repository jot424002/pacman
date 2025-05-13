//
//  main.cpp
//  pacMan
//
//  Created by Ekamjot Kaur on 2/05/25.
//


#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <cstdlib>

// Game constants
const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 650;
const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 20;
const int CELL_SIZE = 30;
const float PI = 3.14159265358979323846f;

// Game state
bool gameOver = false;
int score = 0;
int lives = 3;

// Pac-Man properties
int pacmanX = 10;
int pacmanY = 10;
float pacmanAngle = 0.0f;
int pacmanDir = 0; // 0: right, 1: up, 2: left, 3: down
bool pacmanMouthOpen = true;
int pacmanMouthCounter = 0;
bool pacmanMoving = false; // flag to control movement

// Ghost properties
int ghostX = 15;
int ghostY = 10;
int ghostDir = 0;
bool ghostScared = false;

// Dot properties
struct Dot {
    int x, y;
    bool active;
};

std::vector<Dot> dots;

// Maze layout (1 = wall, 0 = empty)
int maze[GRID_HEIGHT][GRID_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,0,1,1},
    {1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1},
    {1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1},
    {1,1,1,1,0,1,0,0,0,0,0,0,0,1,0,1,1,1,1,1},
    {1,1,1,1,0,1,0,1,1,0,1,1,0,1,0,1,1,1,1,1},
    {0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0},
    {1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1},
    {1,1,1,1,0,1,0,0,0,0,0,0,0,1,0,1,1,1,1,1},
    {1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1,1},
    {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
    {1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1},
    {1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
};

// Function to initialize dots
void initDots() {
    dots.clear();
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (maze[y][x] == 0) {
                // No dots in Pac-Man's starting position
                if (!(x == pacmanX && y == pacmanY)) {
                    Dot dot;
                    dot.x = x;
                    dot.y = y;
                    dot.active = true;
                    dots.push_back(dot);
                }
            }
        }
    }
}

// Function to draw Pac-Man
void drawPacman() {
    glPushMatrix();
    glTranslatef(pacmanX * CELL_SIZE + CELL_SIZE/2, pacmanY * CELL_SIZE + CELL_SIZE/2, 0);
    
    // Set Pac-Man's direction
    if (pacmanDir == 0) glRotatef(0, 0, 0, 1);      // Right
    else if (pacmanDir == 1) glRotatef(270, 0, 0, 1); // Up
    else if (pacmanDir == 2) glRotatef(180, 0, 0, 1); // Left
    else if (pacmanDir == 3) glRotatef(90, 0, 0, 1);  // Down
    
    // Draw Pac-Man as a yellow circle with a mouth
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0);
    
    float mouthAngle = pacmanMouthOpen ? 30.0f : 5.0f;
    
    for (float angle = mouthAngle; angle <= 360 - mouthAngle; angle += 10) {
        float radian = angle * (PI / 180.0f);
        float x = cos(radian) * (CELL_SIZE/2 - 2);
        float y = sin(radian) * (CELL_SIZE/2 - 2);
        glVertex2f(x, y);
    }
    glEnd();
    
    glPopMatrix();
}

// Function to draw Ghost
void drawGhost() {
    glPushMatrix();
    glTranslatef(ghostX * CELL_SIZE + CELL_SIZE/2, ghostY * CELL_SIZE + CELL_SIZE/2, 0);
    
    // Ghost body color
    if (ghostScared) {
        glColor3f(0.0f, 0.0f, 1.0f); // Blue when scared
    } else {
        glColor3f(1.0f, 0.0f, 0.0f); // Red normally
    }
    
    // Draw the ghost body
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0);
    for (float angle = 0; angle <= 180; angle += 10) {
        float radian = angle * (PI / 180.0f);
        float x = cos(radian) * (CELL_SIZE/2 - 2);
        float y = sin(radian) * (CELL_SIZE/2 - 2) + 2;
        glVertex2f(x, y);
    }
    glEnd();
    
    // Draw the bottom part with "tentacles"
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0);
    glVertex2f(CELL_SIZE/2 - 2, 0);
    glVertex2f(CELL_SIZE/2 - 2, -CELL_SIZE/2 + 2);
    glVertex2f(CELL_SIZE/4 - 1, -CELL_SIZE/4);
    glVertex2f(0, -CELL_SIZE/2 + 2);
    glVertex2f(-CELL_SIZE/4 + 1, -CELL_SIZE/4);
    glVertex2f(-CELL_SIZE/2 + 2, -CELL_SIZE/2 + 2);
    glVertex2f(-CELL_SIZE/2 + 2, 0);
    glEnd();
    
    // Draw the eyes
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-CELL_SIZE/6, CELL_SIZE/6);
    for (float angle = 0; angle <= 360; angle += 30) {
        float radian = angle * (PI / 180.0f);
        float x = -CELL_SIZE/6 + cos(radian) * (CELL_SIZE/8);
        float y = CELL_SIZE/6 + sin(radian) * (CELL_SIZE/8);
        glVertex2f(x, y);
    }
    glEnd();
    
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(CELL_SIZE/6, CELL_SIZE/6);
    for (float angle = 0; angle <= 360; angle += 30) {
        float radian = angle * (PI / 180.0f);
        float x = CELL_SIZE/6 + cos(radian) * (CELL_SIZE/8);
        float y = CELL_SIZE/6 + sin(radian) * (CELL_SIZE/8);
        glVertex2f(x, y);
    }
    glEnd();
    
    // Draw pupils
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-CELL_SIZE/6 + CELL_SIZE/20, CELL_SIZE/6);
    for (float angle = 0; angle <= 360; angle += 30) {
        float radian = angle * (PI / 180.0f);
        float x = -CELL_SIZE/6 + CELL_SIZE/20 + cos(radian) * (CELL_SIZE/16);
        float y = CELL_SIZE/6 + sin(radian) * (CELL_SIZE/16);
        glVertex2f(x, y);
    }
    glEnd();
    
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(CELL_SIZE/6 + CELL_SIZE/20, CELL_SIZE/6);
    for (float angle = 0; angle <= 360; angle += 30) {
        float radian = angle * (PI / 180.0f);
        float x = CELL_SIZE/6 + CELL_SIZE/20 + cos(radian) * (CELL_SIZE/16);
        float y = CELL_SIZE/6 + sin(radian) * (CELL_SIZE/16);
        glVertex2f(x, y);
    }
    glEnd();
    
    glPopMatrix();
}

// Function to draw a dot
void drawDot(int x, int y) {
    glColor3f(1.0f, 0.8f, 0.8f);
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glVertex2f(x * CELL_SIZE + CELL_SIZE/2, y * CELL_SIZE + CELL_SIZE/2);
    glEnd();
}

// Function to draw the maze
void drawMaze() {
    glColor3f(0.0f, 0.3f, 0.8f);
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (maze[y][x] == 1) {
                glBegin(GL_QUADS);
                glVertex2f(x * CELL_SIZE, y * CELL_SIZE);
                glVertex2f((x + 1) * CELL_SIZE, y * CELL_SIZE);
                glVertex2f((x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE);
                glVertex2f(x * CELL_SIZE, (y + 1) * CELL_SIZE);
                glEnd();
            }
        }
    }
}
void checkGhostCollision() {
    // Check if Pac-Man and ghost are in the same position
    if (pacmanX == ghostX && pacmanY == ghostY) {
        if (ghostScared) {
            // Eat ghost
            ghostX = 10;  // Reset ghost to starting position
            ghostY = 10;
            ghostScared = false;
            score += 200;
        } else {
            // Lose a life
            lives--;
            
            if (lives <= 0) {
                gameOver = true;
            } else {
                // Reset positions
                pacmanX = 10;
                pacmanY = 10;
                ghostX = 15;
                ghostY = 10;
            }
        }
    }
}

// Function to check collisions with walls
bool checkCollision(int x, int y) {
    // Check if out of bounds
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
        return true;
    }
    
    // Check if wall
    return maze[y][x] == 1;
}

// Function to move Pac-Man one step in the current direction
void movePacman() {
    // Only move if pacmanMoving flag is true (set by keyboard input)
    if (pacmanMoving) {
        int newX = pacmanX;
        int newY = pacmanY;
        
        // Calculate new position based on direction
        if (pacmanDir == 0) newX += 1;       // Right
        else if (pacmanDir == 1) newY -= 1;  // Up (y-axis is inverted in OpenGL)
        else if (pacmanDir == 2) newX -= 1;  // Left
        else if (pacmanDir == 3) newY += 1;  // Down
        
        // Wrap around the screen (tunnel effect)
        if (newX < 0) newX = GRID_WIDTH - 1;
        if (newX >= GRID_WIDTH) newX = 0;
        if (newY < 0) newY = GRID_HEIGHT - 1;
        if (newY >= GRID_HEIGHT) newY = 0;
        
        // Check for collisions with walls
        if (!checkCollision(newX, newY)) {
            pacmanX = newX;
            pacmanY = newY;
            
            // Animate Pac-Man's mouth
            pacmanMouthOpen = !pacmanMouthOpen;
            
            // Check for dot collisions
            for (auto& dot : dots) {
                if (dot.active && pacmanX == dot.x && pacmanY == dot.y) {
                    dot.active = false;
                    score += 10;
                }
            }
            
            // Check for ghost collision after Pac-Man moves
            checkGhostCollision();
        }
        
        // Reset the moving flag so Pac-Man waits for next key press
        pacmanMoving = false;
    }
}

// Function to handle Ghost movement
void moveGhost() {
    // Simple ghost AI
    if (rand() % 10 == 0) {
        // Randomly change direction occasionally
        ghostDir = rand() % 4;
    } else {
        // Chase Pac-Man , otherwise move randomly
        if (rand() % 100 < 60 && !ghostScared) {
            // Chase logic - try to move toward Pac-Man
            if (pacmanX < ghostX) {
                if (!checkCollision(ghostX - 1, ghostY)) ghostDir = 2;
            } else if (pacmanX > ghostX) {
                if (!checkCollision(ghostX + 1, ghostY)) ghostDir = 0;
            } else if (pacmanY < ghostY) {
                if (!checkCollision(ghostX, ghostY - 1)) ghostDir = 1;
            } else if (pacmanY > ghostY) {
                if (!checkCollision(ghostX, ghostY + 1)) ghostDir = 3;
            }
        }
    }
    
    int newX = ghostX;
    int newY = ghostY;
    
    // Move based on direction
    if (ghostDir == 0) newX += 1;
    else if (ghostDir == 1) newY -= 1;
    else if (ghostDir == 2) newX -= 1;
    else if (ghostDir == 3) newY += 1;
    
    // Wrap around the screen (tunnel effect)
    if (newX < 0) newX = GRID_WIDTH - 1;
    if (newX >= GRID_WIDTH) newX = 0;
    if (newY < 0) newY = GRID_HEIGHT - 1;
    if (newY >= GRID_HEIGHT) newY = 0;
    
    // Check for collisions with walls
    if (!checkCollision(newX, newY)) {
        ghostX = newX;
        ghostY = newY;
        
        // Check for ghost collision after the ghost moves
        checkGhostCollision();
    } else {
        // If collision with wall, try another direction
        ghostDir = rand() % 4;
    }
}

// Function to draw the score and lives
void drawHUD() {
    char scoreText[20];
    sprintf(scoreText, "Score: %d", score);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(10, WINDOW_HEIGHT - 20);
    
    for (char* c = scoreText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    char livesText[20];
    sprintf(livesText, "Lives: %d", lives);
    
    glRasterPos2f(WINDOW_WIDTH - 80, WINDOW_HEIGHT - 20);
    
    for (char* c = livesText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    // Display movement instructions
    char instructionsText[] = "Press WASD or arrow keys to move one step";
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(WINDOW_WIDTH/2 - 140, WINDOW_HEIGHT - 20);
    
    for (char* c = instructionsText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    if (gameOver) {
        char gameOverText[] = "GAME OVER - Press R to Restart";
        glColor3f(1.0f, 0.0f, 0.0f);
        glRasterPos2f(WINDOW_WIDTH/2 - 120, WINDOW_HEIGHT/2);
        
        for (char* c = gameOverText; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }
    
    // Check if all dots are eaten
    bool allDotsEaten = true;
    for (const auto& dot : dots) {
        if (dot.active) {
            allDotsEaten = false;
            break;
        }
    }
    
    if (allDotsEaten) {
        char winText[] = "YOU WIN! - Press R to Restart";
        glColor3f(0.0f, 1.0f, 0.0f);
        glRasterPos2f(WINDOW_WIDTH/2 - 120, WINDOW_HEIGHT/2);
        
        for (char* c = winText; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
        gameOver = true;
    }
}

// Function to reset the game
void resetGame() {
    pacmanX = 10;
    pacmanY = 10;
    pacmanDir = 0;
    ghostX = 15;
    ghostY = 10;
    ghostDir = 0;
    ghostScared = false;
    score = 0;
    lives = 3;
    gameOver = false;
    pacmanMoving = false; // Reset the moving flag
    initDots();
}

// Main display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Draw the maze
    drawMaze();
    
    // Draw the dots
    for (const auto& dot : dots) {
        if (dot.active) {
            drawDot(dot.x, dot.y);
        }
    }
    
    // Draw Pac-Man
    drawPacman();
    
    // Draw Ghost
    drawGhost();
    
    // Draw HUD
    drawHUD();
    
    glutSwapBuffers();
}

// Update function
void update(int value) {
    if (!gameOver) {
        movePacman(); // Will only move if pacmanMoving flag is true
        moveGhost();  // Ghost moves on its own schedule
        
        // Additional check in case both characters stop on the same spot
        // after their movements have been processed
        checkGhostCollision();
    }
    
    glutPostRedisplay();
    glutTimerFunc(200, update, 0); // Slower update rate for step-by-step movement
}

// Keyboard function for handling user input
void keyboard(unsigned char key, int x, int y) {
    if (key == 'r' || key == 'R') {
        resetGame();
    }
    
    if (!gameOver) {
        if (key == 'a' || key == 'A') {
            pacmanDir = 2; // Left
            pacmanMoving = true; // Set the moving flag
        } else if (key == 'd' || key == 'D') {
            pacmanDir = 0; // Right
            pacmanMoving = true; // Set the moving flag
        } else if (key == 'w' || key == 'W') {
            pacmanDir = 1; // Up
            pacmanMoving = true; // Set the moving flag
        } else if (key == 's' || key == 'S') {
            pacmanDir = 3; // Down
            pacmanMoving = true; // Set the moving flag
        }
    }
}

// Arrow key function for handling user input
void specialKeys(int key, int x, int y) {
    if (!gameOver) {
        if (key == GLUT_KEY_LEFT) {
            pacmanDir = 2; // Left
            pacmanMoving = true; // Set the moving flag
        } else if (key == GLUT_KEY_RIGHT) {
            pacmanDir = 0; // Right
            pacmanMoving = true; // Set the moving flag
        } else if (key == GLUT_KEY_UP) {
            pacmanDir = 1; // Up
            pacmanMoving = true; // Set the moving flag
        } else if (key == GLUT_KEY_DOWN) {
            pacmanDir = 3; // Down
            pacmanMoving = true; // Set the moving flag
        }
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Pac-Man Game with Step Movement");
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    
    // Initialize dots
    initDots();
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(200, update, 0); // Slower initial update rate
    
    glutMainLoop();
    
    return 0;
}
