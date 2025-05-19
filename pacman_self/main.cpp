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

// Ghost structure
struct Ghost {
    int x, y;
    int dir;
    bool scared;
    int initialX, initialY;
    int moveCounter;  // Added to control movement speed
};

// Ghosts
std::vector<Ghost> ghosts;

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
    
    if (pacmanDir == 0) glRotatef(0, 0, 0, 1);      // Right
    else if (pacmanDir == 1) glRotatef(270, 0, 0, 1); // Up
    else if (pacmanDir == 2) glRotatef(180, 0, 0, 1); // Left
    else if (pacmanDir == 3) glRotatef(90, 0, 0, 1);  // Down
    
    float mouthAngle = pacmanMouthOpen ? 30.0f : 5.0f;
    
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.9f, 0.9f, 0.0f);
    glVertex2f(0, 0);
    glColor3f(1.0f, 1.0f, 0.0f);
    for (float angle = mouthAngle; angle <= 360 - mouthAngle; angle += 5) {
        float radian = angle * (PI / 180.0f);
        float x = cos(radian) * (CELL_SIZE/2 - 2);
        float y = sin(radian) * (CELL_SIZE/2 - 2);
        glVertex2f(x, y);
    }
    glEnd();
    
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    for (float angle = mouthAngle; angle <= 360 - mouthAngle; angle += 5) {
        float radian = angle * (PI / 180.0f);
        float x = cos(radian) * (CELL_SIZE/2 - 1);
        float y = sin(radian) * (CELL_SIZE/2 - 1);
        glVertex2f(x, y);
    }
    glEnd();
    
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    float eyeX = CELL_SIZE/8;
    float eyeY = -CELL_SIZE/6;
    glVertex2f(eyeX, eyeY);
    for (float angle = 0; angle <= 360; angle += 30) {
        float radian = angle * (PI / 180.0f);
        float x = eyeX + cos(radian) * (CELL_SIZE/16);
        float y = eyeY + sin(radian) * (CELL_SIZE/16);
        glVertex2f(x, y);
    }
    glEnd();
    
    glPopMatrix();
}

// Function to draw Ghost
void drawGhost(int x, int y, int colorIndex) {
    glPushMatrix();
    glTranslatef(x * CELL_SIZE + CELL_SIZE/2, y * CELL_SIZE + CELL_SIZE/2, 0);
    
    // Set color based on ghost index (mimicking classic Pac-Man colors)
    if (colorIndex == 0) glColor3f(0.9f, 0.2f, 0.2f); // Red
    else if (colorIndex == 1) glColor3f(1.0f, 0.5f, 0.5f); // Pink
    else if (colorIndex == 2) glColor3f(0.0f, 1.0f, 1.0f); // Cyan
    else if (colorIndex == 3) glColor3f(1.0f, 0.5f, 0.0f); // Orange
    
    glBegin(GL_POLYGON);
    for (float angle = 0; angle <= 180; angle += 5) {
        float radian = angle * (PI / 180.0f);
        float dx = cos(radian) * (CELL_SIZE/2);
        float dy = sin(radian) * (CELL_SIZE/2);
        glVertex2f(dx, dy);
    }
    glVertex2f(-CELL_SIZE/2, 0);
    glVertex2f(-CELL_SIZE/3, -CELL_SIZE/6);
    glVertex2f(-CELL_SIZE/6, 0);
    glVertex2f(0, -CELL_SIZE/6);
    glVertex2f(CELL_SIZE/6, 0);
    glVertex2f(CELL_SIZE/3, -CELL_SIZE/6);
    glVertex2f(CELL_SIZE/2, 0);
    glEnd();
    
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    for (float angle = 0; angle <= 180; angle += 10) {
        float radian = angle * (PI / 180.0f);
        float dx = cos(radian) * (CELL_SIZE/2);
        float dy = sin(radian) * (CELL_SIZE/2);
        glVertex2f(dx, dy);
    }
    glVertex2f(-CELL_SIZE/2, 0);
    glVertex2f(-CELL_SIZE/3, -CELL_SIZE/6);
    glVertex2f(-CELL_SIZE/6, 0);
    glVertex2f(0, -CELL_SIZE/6);
    glVertex2f(CELL_SIZE/6, 0);
    glVertex2f(CELL_SIZE/3, -CELL_SIZE/6);
    glVertex2f(CELL_SIZE/2, 0);
    glEnd();
    
    glColor3f(1.0f, 0.6f, 0.6f);
    float highlightSize = CELL_SIZE / 10;
    float highlightY = CELL_SIZE / 3;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-CELL_SIZE/4, highlightY);
    for (float angle = 0; angle <= 360; angle += 30) {
        float radian = angle * (PI / 180.0f);
        glVertex2f(-CELL_SIZE/4 + cos(radian) * highlightSize, highlightY + sin(radian) * highlightSize);
    }
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(CELL_SIZE/4, highlightY);
    for (float angle = 0; angle <= 360; angle += 30) {
        float radian = angle * (PI / 180.0f);
        glVertex2f(CELL_SIZE/4 + cos(radian) * highlightSize, highlightY + sin(radian) * highlightSize);
    }
    glEnd();
    
    float eyeY = -CELL_SIZE/6;
    float eyeSizeX = CELL_SIZE/8;
    float eyeSizeY = CELL_SIZE/10;
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-CELL_SIZE/4, eyeY);
    for (float angle = 0; angle <= 360; angle += 10) {
        float radian = angle * (PI / 180.0f);
        glVertex2f(-CELL_SIZE/4 + cos(radian) * eyeSizeX, eyeY + sin(radian) * eyeSizeY);
    }
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(CELL_SIZE/4, eyeY);
    for (float angle = 0; angle <= 360; angle += 10) {
        float radian = angle * (PI / 180.0f);
        glVertex2f(CELL_SIZE/4 + cos(radian) * eyeSizeX, eyeY + sin(radian) * eyeSizeY);
    }
    glEnd();
    
    glColor3f(0.0f, 0.0f, 0.5f);
    float pupilSize = CELL_SIZE/20;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-CELL_SIZE/4, eyeY);
    for (float angle = 0; angle <= 360; angle += 10) {
        float radian = angle * (PI / 180.0f);
        glVertex2f(-CELL_SIZE/4 + cos(radian) * pupilSize, eyeY + sin(radian) * pupilSize);
    }
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(CELL_SIZE/4, eyeY);
    for (float angle = 0; angle <= 360; angle += 10) {
        float radian = angle * (PI / 180.0f);
        glVertex2f(CELL_SIZE/4 + cos(radian) * pupilSize, eyeY + sin(radian) * pupilSize);
    }
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    float shineSize = pupilSize / 3;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-CELL_SIZE/4 + pupilSize / 2, eyeY + pupilSize / 2);
    for (float angle = 0; angle <= 360; angle += 30) {
        float radian = angle * (PI / 180.0f);
        glVertex2f(-CELL_SIZE/4 + pupilSize / 2 + cos(radian) * shineSize, eyeY + pupilSize / 2 + sin(radian) * shineSize);
    }
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(CELL_SIZE/4 + pupilSize / 2, eyeY + pupilSize / 2);
    for (float angle = 0; angle <= 360; angle += 30) {
        float radian = angle * (PI / 180.0f);
        glVertex2f(CELL_SIZE/4 + pupilSize / 2 + cos(radian) * shineSize, eyeY + pupilSize / 2 + sin(radian) * shineSize);
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
    float outerColor[3] = {0.0f, 0.2f, 0.7f};
    float innerColor[3] = {0.2f, 0.4f, 0.9f};
    float wallInset = 2.0f;
    
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (maze[y][x] == 1) {
                bool wallUp = (y > 0) ? maze[y-1][x] == 1 : false;
                bool wallDown = (y < GRID_HEIGHT-1) ? maze[y+1][x] == 1 : false;
                bool wallLeft = (x > 0) ? maze[y][x-1] == 1 : false;
                bool wallRight = (x < GRID_WIDTH-1) ? maze[y][x+1] == 1 : false;
                
                glBegin(GL_QUADS);
                glColor3fv(outerColor);
                glVertex2f(x * CELL_SIZE + wallInset, y * CELL_SIZE + wallInset);
                glVertex2f((x + 1) * CELL_SIZE - wallInset, y * CELL_SIZE + wallInset);
                glVertex2f((x + 1) * CELL_SIZE - wallInset, (y + 1) * CELL_SIZE - wallInset);
                glVertex2f(x * CELL_SIZE + wallInset, (y + 1) * CELL_SIZE - wallInset);
                glEnd();
                
                if (wallUp) {
                    glBegin(GL_QUADS);
                    glColor3fv(innerColor);
                    glVertex2f(x * CELL_SIZE + wallInset, y * CELL_SIZE);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset, y * CELL_SIZE);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset, y * CELL_SIZE + wallInset);
                    glVertex2f(x * CELL_SIZE + wallInset, y * CELL_SIZE + wallInset);
                    glEnd();
                }
                
                if (wallDown) {
                    glBegin(GL_QUADS);
                    glColor3fv(innerColor);
                    glVertex2f(x * CELL_SIZE + wallInset, (y + 1) * CELL_SIZE - wallInset);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset, (y + 1) * CELL_SIZE - wallInset);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset, (y + 1) * CELL_SIZE);
                    glVertex2f(x * CELL_SIZE + wallInset, (y + 1) * CELL_SIZE);
                    glEnd();
                }
                
                if (wallLeft) {
                    glBegin(GL_QUADS);
                    glColor3fv(innerColor);
                    glVertex2f(x * CELL_SIZE, y * CELL_SIZE + wallInset);
                    glVertex2f(x * CELL_SIZE + wallInset, y * CELL_SIZE + wallInset);
                    glVertex2f(x * CELL_SIZE + wallInset, (y + 1) * CELL_SIZE - wallInset);
                    glVertex2f(x * CELL_SIZE, (y + 1) * CELL_SIZE - wallInset);
                    glEnd();
                }
                
                if (wallRight) {
                    glBegin(GL_QUADS);
                    glColor3fv(innerColor);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset, y * CELL_SIZE + wallInset);
                    glVertex2f((x + 1) * CELL_SIZE, y * CELL_SIZE + wallInset);
                    glVertex2f((x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE - wallInset);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset, (y + 1) * CELL_SIZE - wallInset);
                    glEnd();
                }
                
                if (wallLeft && wallUp) {
                    glBegin(GL_QUADS);
                    glColor3fv(innerColor);
                    glVertex2f(x * CELL_SIZE, y * CELL_SIZE);
                    glVertex2f(x * CELL_SIZE + wallInset, y * CELL_SIZE);
                    glVertex2f(x * CELL_SIZE + wallInset, y * CELL_SIZE + wallInset);
                    glVertex2f(x * CELL_SIZE, y * CELL_SIZE + wallInset);
                    glEnd();
                }
                
                if (wallRight && wallUp) {
                    glBegin(GL_QUADS);
                    glColor3fv(innerColor);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset, y * CELL_SIZE);
                    glVertex2f((x + 1) * CELL_SIZE, y * CELL_SIZE);
                    glVertex2f((x + 1) * CELL_SIZE, y * CELL_SIZE + wallInset);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset, y * CELL_SIZE + wallInset);
                    glEnd();
                }
                
                if (wallLeft && wallDown) {
                    glBegin(GL_QUADS);
                    glColor3fv(innerColor);
                    glVertex2f(x * CELL_SIZE, (y + 1) * CELL_SIZE - wallInset);
                    glVertex2f(x * CELL_SIZE + wallInset, (y + 1) * CELL_SIZE - wallInset);
                    glVertex2f(x * CELL_SIZE + wallInset, (y + 1) * CELL_SIZE);
                    glVertex2f(x * CELL_SIZE, (y + 1) * CELL_SIZE);
                    glEnd();
                }
                
                if (wallRight && wallDown) {
                    glBegin(GL_QUADS);
                    glColor3fv(innerColor);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset, (y + 1) * CELL_SIZE - wallInset);
                    glVertex2f((x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE - wallInset);
                    glVertex2f((x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset, (y + 1) * CELL_SIZE);
                    glEnd();
                }
                
                float highlightWidth = wallInset / 2.0f;
                glColor3f(0.3f, 0.5f, 1.0f);
                if (!wallUp) {
                    glBegin(GL_QUADS);
                    glVertex2f(x * CELL_SIZE + wallInset, y * CELL_SIZE + wallInset);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset, y * CELL_SIZE + wallInset);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset - highlightWidth, y * CELL_SIZE + wallInset + highlightWidth);
                    glVertex2f(x * CELL_SIZE + wallInset + highlightWidth, y * CELL_SIZE + wallInset + highlightWidth);
                    glEnd();
                }
                
                if (!wallLeft) {
                    glBegin(GL_QUADS);
                    glVertex2f(x * CELL_SIZE + wallInset, y * CELL_SIZE + wallInset);
                    glVertex2f(x * CELL_SIZE + wallInset + highlightWidth, y * CELL_SIZE + wallInset + highlightWidth);
                    glVertex2f(x * CELL_SIZE + wallInset + highlightWidth, (y + 1) * CELL_SIZE - wallInset - highlightWidth);
                    glVertex2f(x * CELL_SIZE + wallInset, (y + 1) * CELL_SIZE - wallInset);
                    glEnd();
                }
                
                glColor3f(0.0f, 0.1f, 0.5f);
                if (!wallDown) {
                    glBegin(GL_QUADS);
                    glVertex2f(x * CELL_SIZE + wallInset, (y + 1) * CELL_SIZE - wallInset);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset, (y + 1) * CELL_SIZE - wallInset);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset - highlightWidth, (y + 1) * CELL_SIZE - wallInset - highlightWidth);
                    glVertex2f(x * CELL_SIZE + wallInset + highlightWidth, (y + 1) * CELL_SIZE - wallInset - highlightWidth);
                    glEnd();
                }
                
                if (!wallRight) {
                    glBegin(GL_QUADS);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset, y * CELL_SIZE + wallInset);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset - highlightWidth, y * CELL_SIZE + wallInset + highlightWidth);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset - highlightWidth, (y + 1) * CELL_SIZE - wallInset - highlightWidth);
                    glVertex2f((x + 1) * CELL_SIZE - wallInset, (y + 1) * CELL_SIZE - wallInset);
                    glEnd();
                }
            }
        }
    }
    
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (maze[y][x] == 0) {
                glBegin(GL_QUADS);
                glColor3f(0.0f, 0.0f, 0.2f);
                glVertex2f(x * CELL_SIZE, y * CELL_SIZE);
                glVertex2f((x + 1) * CELL_SIZE, y * CELL_SIZE);
                glVertex2f((x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE);
                glVertex2f(x * CELL_SIZE, (y + 1) * CELL_SIZE);
                glEnd();
                
                glLineWidth(0.5f);
                glColor4f(0.1f, 0.1f, 0.3f, 0.3f);
                glBegin(GL_LINE_LOOP);
                glVertex2f(x * CELL_SIZE, y * CELL_SIZE);
                glVertex2f((x + 1) * CELL_SIZE, y * CELL_SIZE);
                glVertex2f((x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE);
                glVertex2f(x * CELL_SIZE, (y + 1) * CELL_SIZE);
                glEnd();
            }
        }
    }
}

// Function to handle ghost collisions
void checkGhostCollisions() {
    for (auto& ghost : ghosts) {
        if (pacmanX == ghost.x && pacmanY == ghost.y) {
            if (ghost.scared) {
                ghost.x = ghost.initialX;
                ghost.y = ghost.initialY;
                ghost.scared = false;
                score += 200;
            } else {
                lives--;
                if (lives <= 0) {
                    gameOver = true;
                } else {
                    pacmanX = 10;
                    pacmanY = 10;
                    for (auto& g : ghosts) {
                        g.x = g.initialX;
                        g.y = g.initialY;
                    }
                }
            }
            return; // Handle one collision at a time
        }
    }
}

// Function to check collisions with walls
bool checkCollision(int x, int y) {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
        return true;
    }
    return maze[y][x] == 1;
}

// Function to move Pac-Man one step in the current direction
void movePacman() {
    if (pacmanMoving) {
        int newX = pacmanX;
        int newY = pacmanY;
        
        if (pacmanDir == 0) newX += 1;
        else if (pacmanDir == 1) newY -= 1;
        else if (pacmanDir == 2) newX -= 1;
        else if (pacmanDir == 3) newY += 1;
        
        if (newX < 0) newX = GRID_WIDTH - 1;
        if (newX >= GRID_WIDTH) newX = 0;
        if (newY < 0) newY = GRID_HEIGHT - 1;
        if (newY >= GRID_HEIGHT) newY = 0;
        
        if (!checkCollision(newX, newY)) {
            pacmanX = newX;
            pacmanY = newY;
            
            pacmanMouthOpen = !pacmanMouthOpen;
            
            for (auto& dot : dots) {
                if (dot.active && pacmanX == dot.x && pacmanY == dot.y) {
                    dot.active = false;
                    score += 10;
                }
            }
            
            checkGhostCollisions();
        }
        
        pacmanMoving = false;
    }
}

// Function to handle Ghost movement
void moveGhost(Ghost& ghost) {
    if (rand() % 10 == 0) {
        ghost.dir = rand() % 4;
    } else {
        if (rand() % 100 < 60 && !ghost.scared) {
            if (pacmanX < ghost.x) {
                if (!checkCollision(ghost.x - 1, ghost.y)) ghost.dir = 2;
            } else if (pacmanX > ghost.x) {
                if (!checkCollision(ghost.x + 1, ghost.y)) ghost.dir = 0;
            } else if (pacmanY < ghost.y) {
                if (!checkCollision(ghost.x, ghost.y - 1)) ghost.dir = 1;
            } else if (pacmanY > ghost.y) {
                if (!checkCollision(ghost.x, ghost.y + 1)) ghost.dir = 3;
            }
        }
    }
    
    int newX = ghost.x;
    int newY = ghost.y;
    
    if (ghost.dir == 0) newX += 1;
    else if (ghost.dir == 1) newY -= 1;
    else if (ghost.dir == 2) newX -= 1;
    else if (ghost.dir == 3) newY += 1;
    
    if (newX < 0) newX = GRID_WIDTH - 1;
    if (newX >= GRID_WIDTH) newX = 0;
    if (newY < 0) newY = GRID_HEIGHT - 1;
    if (newY >= GRID_HEIGHT) newY = 0;
    
    if (!checkCollision(newX, newY)) {
        ghost.x = newX;
        ghost.y = newY;
        
        checkGhostCollisions();
    } else {
        ghost.dir = rand() % 4;
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
    
    char instructionsText[] = "Press WASD or arrow keys to move one step";
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(WINDOW_WIDTH/2 - 140, WINDOW_HEIGHT - 20);
    
    for (char* c = instructionsText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    if (gameOver) {
        char* gameOverText = "GAME OVER - Press R to Restart";
        glColor3f(1.0f, 0.0f, 0.0f);
        glRasterPos2f(WINDOW_WIDTH/2 - 120, WINDOW_HEIGHT/2 + 20);
        
        for (char* c = gameOverText; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }

    bool allDotsEaten = true;
    for (const auto& dot : dots) {
        if (dot.active) {
            allDotsEaten = false;
            break;
        }
    }

    if (allDotsEaten) {
        char* winText = "YOU WIN! - Press R to Restart";
        glColor3f(0.0f, 1.0f, 0.0f);
        glRasterPos2f(WINDOW_WIDTH/2 - 120, WINDOW_HEIGHT/2 - 20);
        
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
    score = 0;
    lives = 3;
    gameOver = false;
    pacmanMoving = false;
    ghosts.clear();
    // Initialize four ghosts at different empty positions
    ghosts.push_back({15, 10, 0, false, 15, 10}); // Original ghost position
    ghosts.push_back({4, 10, 0, false, 4, 10});
//    ghosts.push_back({10, 5, 0, false, 10, 5});
//    ghosts.push_back({10, 15, 0, false, 10, 15});
    initDots();
}

// Main display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    drawMaze();
    
    for (const auto& dot : dots) {
        if (dot.active) {
            drawDot(dot.x, dot.y);
        }
    }
    
    drawPacman();
    
    // Draw all ghosts
    for (size_t i = 0; i < ghosts.size(); i++) {
        drawGhost(ghosts[i].x, ghosts[i].y, i);
    }
    
    drawHUD();
    
    glutSwapBuffers();
}

// Update function
void update(int value) {
    if (!gameOver) {
        movePacman();  // Pac-Man moves every update
        for (auto& ghost : ghosts) {
            ghost.moveCounter++;
            if (ghost.moveCounter >= 2) {  // Ghosts move every 2 updates
                moveGhost(ghost);
                ghost.moveCounter = 0;
            }
        }
        checkGhostCollisions();  // Collision detection remains unaffected
    }
    glutPostRedisplay();
    glutTimerFunc(200, update, 0);  // Update every 200ms
}

// Keyboard function for handling user input
void keyboard(unsigned char key, int x, int y) {
    if (key == 'r' || key == 'R') {
        resetGame();
    }
    
    if (!gameOver) {
        if (key == 'a' || key == 'A') {
            pacmanDir = 2;
            pacmanMoving = true;
        } else if (key == 'd' || key == 'D') {
            pacmanDir = 0;
            pacmanMoving = true;
        } else if (key == 'w' || key == 'W') {
            pacmanDir = 1;
            pacmanMoving = true;
        } else if (key == 's' || key == 'S') {
            pacmanDir = 3;
            pacmanMoving = true;
        }
    }
}

// Arrow key function for handling user input
void specialKeys(int key, int x, int y) {
    if (!gameOver) {
        if (key == GLUT_KEY_LEFT) {
            pacmanDir = 2;
            pacmanMoving = true;
        } else if (key == GLUT_KEY_RIGHT) {
            pacmanDir = 0;
            pacmanMoving = true;
        } else if (key == GLUT_KEY_UP) {
            pacmanDir = 1;
            pacmanMoving = true;
        } else if (key == GLUT_KEY_DOWN) {
            pacmanDir = 3;
            pacmanMoving = true;
        }
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Pac-Man");
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    
    resetGame(); // Initialize game state with multiple ghosts
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(200, update, 0);
    
    glutMainLoop();
    
    return 0;
}
