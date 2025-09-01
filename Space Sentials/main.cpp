#include <windows.h>
#include <GL/glut.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <cstdlib> // For random number generation
#include <ctime>   // For seeding random number generator
#include <math.h>
#pragma comment(lib, "winmm.lib")
using namespace std;
# define M_PI           3.14159265358979323846
const float PI = 3.14159265f;

// Define constants for spaceship health and enemy speed
const int MAX_HEALTH = 100;
const float ENEMY_SPEED = 0.5f;
const int BOSS_HEALTH = 50; // Health of the boss
bool bossMovingRight = true; // Variable to track boss movement direction
//class Bullet;

//Defining the boss
bool bossActive = false;
int bossHealth = 25; // Health of the boss



// Global variables for score and health
int score = 0;
int health = 10;
int level = 1;

bool gameScreen = false;



int randomInRange(int min, int max) {
    return rand() % (max - min + 1) + min;
}

//Defining Enemy
struct Enemy {
    float x, y;
    bool active;
};


struct Boss {
    float x, y;
    int health;
    bool active;
};
Boss boss;


std::vector<Enemy> enemies;
int spaceshipHealth = MAX_HEALTH;

// Initial position of the Boss
GLfloat bossXPosition = 0;
GLfloat bossYPosition = 0;


// Initial position of the spaceship
GLfloat xPosition = 0;
GLfloat yPosition = 0;
GLfloat bulletY = 0.0f;

//Initially the shooting will be off
bool shooting = false;
//pairing the bullets
std::vector<std::pair<GLfloat, GLfloat>> bullets;

// Forward declaration
//bool bulletCollidesWithBoss(const Bullet& bullet, const Boss& boss);

//Text writing function over ridding
void renderBitmapString(float x, float y, float z, void *font, char *string) {
    char *c;
    glRasterPos3f(x, y,z);
    for (c=string; *c != '\0'; c++)
    {
        glutBitmapCharacter(font, *c);
    }
}


void cockpit();
void soundGame()
{

    //PlaySound("a.wav", NULL, SND_ASYNC|SND_FILENAME);
    PlaySound("Space Invaders - Space Invaders.wav", NULL,SND_ASYNC|SND_FILENAME|SND_LOOP);

}

void soundBullet()
{

    PlaySound("Laser Gun Sound Effect.wav", NULL,SND_ASYNC|SND_FILENAME|SND_LOOP);

}






//Bullet class
class Bullet {
public:
    GLfloat x;
    GLfloat y;
    Bullet(GLfloat x, GLfloat y) : x(x), y(y) {}
};


void drawFilledCircle(float x, float y, float radius, int numSegments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // Center of circle
    for(int i = 0; i <= numSegments; i++) {
        float theta = 2.0f * PI * float(i) / float(numSegments);
        float xPos = x + radius * cosf(theta);
        float yPos = y + radius * sinf(theta);
        glVertex2f(xPos, yPos);
    }
    glEnd();
}


// Function to draw and fill the heart
void drawHeart() {
    // Clear the window with current clearing color
    //glClear(GL_COLOR_BUFFER_BIT);
    //glPushMatrix();

    // Set color to red
    glColor3f(1.0, 0.0, 0.0);

    // Begin polygon for filling
    glBegin(GL_POLYGON);

    // Number of points
    int numPoints = 100;
    for(int i = 0; i <= numPoints; i++) {
        // Parameter t, ranging from 0 to 2*PI
        float t = (2 * M_PI / numPoints) * i;

        // Parametric equations of the heart
        float x = 16 * pow(sin(t), 3);
        float y = 13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t);

        // Scale down and adjust to window coordinates
        x *= 0.2; // Adjust scaling to fit in the [-100,100] range
        y *= 0.2; // Adjust scaling to fit in the [-100,100] range

        // Plot the point
        glVertex2f(x, y);
    }

    // End polygon
    glEnd();
    //glPopMatrix();

    // Execute OpenGL functions that are in the queue just to be safe
    //glFlush();
}



//Defining point
struct Point {
    float x, y;
};

//defining Laser
struct Laser {
    Point position;
    float speed;
    bool active;
};


//drawing Random Star Function
void drawRandomStar(float minSize, float maxSize, float minX, float maxX, float minY, float maxY) {
    // Generate random size and position for the star
    float size = minSize + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (maxSize - minSize));
    float x = minX + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (maxX - minX));
    float y = minY + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (maxY - minY));

    // Generate random color for the star
    float r = 1.0;//static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float g = 1.0;//static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float b = 1.0;//static_cast<float>(rand()) / static_cast<float>(RAND_MAX);


    // Draw the star as a rectangle
    glColor3f(r,g,b); // Set color to white
    glBegin(GL_POLYGON);
    float controlsize=5.5;
    /*
    glVertex2f(x - size / controlsize, y + size / controlsize);
    glVertex2f(x + size / controlsize, y + size / controlsize);
    glVertex2f(x + size / controlsize, y - size / controlsize);
    glVertex2f(x - size / controlsize, y - size / controlsize);
    */


    glVertex2f(x - size / controlsize, y);
    glVertex2f(x, y + size / controlsize);
    glVertex2f(x + size / controlsize, y);
    glVertex2f(x, y - size / controlsize);
    glEnd();
}


//Drawing Starry sky function
void drawStarrySky(int numStars, float minSize, float maxSize, float minX, float maxX, float minY, float maxY) {
    // Draw multiple stars to create a starry effect
    for (int i = 0; i < numStars; ++i) {
        //glRotated(30,)
        glPushMatrix();
        drawRandomStar(minSize, maxSize, minX, maxX, minY, maxY);
        glPopMatrix();
    }
}




class Star {
private:
    float x, y, z;
    float pz;

public:
    Star() {
        x = randomInRange(-100, 100); // Adjusted for the range -100 to 100
        y = randomInRange(-100, 100); // Adjusted for the range -100 to 100
        z = randomInRange(1, 200); // Adjusted for the range 1 to 200
        pz = z;
    }

    void update(float speed) {
        y -= speed; // Move vertically
        if (y < -100) {
            y = 100; // Reset to the top
            x = randomInRange(-100, 100); // Randomize x position
            z = randomInRange(1, 200); // Randomize z position
            pz = z;
        }
    }

    void show() const {
        glColor3f(1.0, 1.0, 1.0); // Set star color to white
        glPointSize(2.0);

        float sx = x / z * 100; // Adjust x based on perspective
        float sy = y / z * 100; // Adjust y based on perspective

        glBegin(GL_POINTS);
        glVertex2f(sx, sy);
        glEnd();
    }
};

std::vector<Star> stars;
float speed;

void drawStars() {
    for (auto& star : stars) {
        star.update(speed);
        star.show();
    }
}







//Initializing Spaceship function and draw bullets function
void spaceShip();
void drawBullets();
void drawBoss();
void winnerScreen();


// Function to draw the score board
// Function to draw the score board using renderBitmapString
void drawScoreBoard() {
    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white
    float x = -95.0f; // x-coordinate of score board
    float y = 85.0f; // y-coordinate of score board
    float z = 0.0f; // z-coordinate of score board

    glPushMatrix();
    glTranslatef(-78.0,85.0,1);
    glColor3f(0.0/255,0.0/255,255.0/255.0);
    glBegin(GL_POLYGON);
    glVertex2f(-15.0, 10.0);
    glVertex2f(15.0, 10.0);
    glVertex2f(15.0, 5.0);
    glVertex2f(20.0, 5.0);
    glVertex2f(20.0, -5.0);
    glVertex2f(15.0, -5.0);
    glVertex2f(15.0, -10.0);
    glVertex2f(-15.0, -10.0);
    glVertex2f(-15.0, -5.0);
    glVertex2f(-20.0, -5.0);
    glVertex2f(-20.0, 5.0);
    glVertex2f(-15.0, 5.0);
    glEnd();
    drawFilledCircle(-15.0,5.0,5.0,1000);
    drawFilledCircle(15.0,5.0,5.0,1000);
    drawFilledCircle(15.0,-5.0,5.0,1000);
    drawFilledCircle(-15.0,-5.0,5.0,1000);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-78.0,85.0,1);
    glScalef(0.9,0.9,1.0);
    glColor3f(255.0/255,0.0/255,0.0);
    glBegin(GL_POLYGON);
    glVertex2f(-15.0, 10.0);
    glVertex2f(15.0, 10.0);
    glVertex2f(15.0, 5.0);
    glVertex2f(20.0, 5.0);
    glVertex2f(20.0, -5.0);
    glVertex2f(15.0, -5.0);
    glVertex2f(15.0, -10.0);
    glVertex2f(-15.0, -10.0);
    glVertex2f(-15.0, -5.0);
    glVertex2f(-20.0, -5.0);
    glVertex2f(-20.0, 5.0);
    glVertex2f(-15.0, 5.0);
    glEnd();
    drawFilledCircle(-15.0,5.0,5.0,1000);
    drawFilledCircle(15.0,5.0,5.0,1000);
    drawFilledCircle(15.0,-5.0,5.0,1000);
    drawFilledCircle(-15.0,-5.0,5.0,1000);
    glPopMatrix();


    char scoreStr[50];
    char levelStr[50];
    char healthStr[50];
    sprintf(scoreStr, "Score: %d", score);
    sprintf(levelStr, "Level: %d", level);
    sprintf(healthStr, "Health: %d", health);
    glColor3f(1.0,1.0,1.0);
    renderBitmapString(x+2, y+3, z, GLUT_BITMAP_HELVETICA_18, scoreStr);
    glColor3f(0.0,1.0,0.0);
    renderBitmapString(x+2, y-5, z, GLUT_BITMAP_HELVETICA_18, levelStr);
    glColor3f(255.0/255.0,215.0/255.0,0.0/255.0);
    renderBitmapString(x+17, y+3, z, GLUT_BITMAP_HELVETICA_18, healthStr);
}

// Function to draw the health bar
void drawHealthBar() {
    glColor3f(0.0f, 1.0f, 1.0f); // Set color to red for health bar
    float startX = 30.0f; // Starting x-coordinate of health bar
    float startY = 90.0f; // Starting y-coordinate of health bar
    float spacing = 2.0f; // Spacing between health blocks
    float blockSize = 5.0f; // Size of each health block

    for (int i = 0; i < health; ++i) {

        // Calculate the position of each heart without using glTranslatef
        float heartPosX = startX + i * (spacing + blockSize);
        float heartPosY = startY;

        // Draw the heart at the calculated position
        glPushMatrix();
        glTranslatef(heartPosX, heartPosY, 0);
        drawHeart();
        glPopMatrix();

    }
}

// Function to update health when the spaceship collides with an enemy
void updateHealth() {
    if (health > 0) {
        health--; // Decrease health when collision occurs
    }

    if (score>=5 && score<=10){
        level = 2;
    }
    else{
        level = 3;
    }

}

//Function to randomly spawn enemies
void spawnEnemy() {
    // Randomly spawn an enemy at the top of the screen
    Enemy enemy;
    enemy.x = static_cast<float>(rand() % 200 - 100); // Random x-coordinate within range
    enemy.y = 100.0f; // Spawn at top of the screen
    enemy.active = true;
    enemies.push_back(enemy);
}

//Randomly spawn Boss
void spawnBoss() {
    boss.x = static_cast<float>(rand() % 200 - 100); // Random x-coordinate within range
    boss.y = 80.0f; // Fixed upper coordinate
    boss.health = BOSS_HEALTH;
    boss.active = true;
}

/*
//Checking for bullet's collision with boss
bool bulletCollidesWithBoss(const Bullet& bullet, const Boss& boss) {
    // Calculate the boundaries of the boss's bounding box
    float bossLeft = boss.x - boss.width / 2;   // Left edge of the boss
    float bossRight = boss.x + boss.width / 2;  // Right edge of the boss
    float bossTop = boss.y + boss.height / 2;   // Top edge of the boss
    float bossBottom = boss.y - boss.height / 2;// Bottom edge of the boss

    // Check if the bullet's position is within the boundaries of the boss's bounding box
    if (bullet.x >= bossLeft && bullet.x <= bossRight &&
        bullet.y >= bossBottom && bullet.y <= bossTop) {
        // Collision detected
        return true;
    }

    // No collision detected
    return false;
}
*/

void switchToRainScreen();

//Update function for enemies
void updateEnemies() {
    if(gameScreen==true){
    for (auto& enemy : enemies) {
        if (enemy.active) {
            // Move enemies down
            enemy.y -= ENEMY_SPEED;

            // Check collision with spaceship
            if (enemy.y < yPosition && enemy.x > xPosition - 10 && enemy.x < xPosition + 10) {
                // Collision with spaceship
                enemy.active = false;
                spaceshipHealth -= 10; // Decrease health
                updateHealth();
                if (spaceshipHealth <= 0) {
                    // Game over
                    std::cout << "Game Over!" << std::endl;//game over display will go here
                    switchToRainScreen();
                    //exit(0);
                }
                 // Remove enemy
            }
            /*


            // Check collision with bullets
            for (auto& bullet : bullets) {
                if (enemy.active && bullet.second > enemy.y && bullet.first > enemy.x - 10 && bullet.first < enemy.x + 10) {
                    // Collision with bullet
                    // Increase points or do whatever you need to do
                    // For now, let's just deactivate the enemy and remove the bullet
                    enemy.active = false;
                    bullet.second = 2.0f; // Move bullet out of screen
                }
            }
            */
            // Check collision with bullets
            for (auto& bullet : bullets) {
                 if (enemy.active && bullet.second > enemy.y + 10 && bullet.first > enemy.x - 10 && bullet.first < enemy.x + 10) {
                    // Collision with bullet
                    // Increase points or do whatever you need to do
                    score++;
                    // For now, let's just deactivate the enemy and remove the bullet
                    enemy.active = false;
                    bullet.second = 2.0f; // Move bullet out of screen

                    // Remove the bullet from the vector
                    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                    [&](const std::pair<GLfloat, GLfloat>& b) {
                    return b.first == bullet.first && b.second == bullet.second;
                    }), bullets.end());

                    // Break out of the loop since the bullet can only hit one enemy
                    break;
                 }
            }

        }
    }

/*
    // Check if the boss is active
    if (bossActive) {
        // Update boss position
        // Check for collisions with bullets and decrement boss health
        for (auto& bullet : bullets) {
            if (bulletCollidesWithBoss(bullet, boss)) {
                bossHealth--; // Decrease boss health
                // Remove bullet or mark it for removal
            }
        }

        // Check if boss health reaches zero
        if (bossHealth <= 0) {
            bossActive = false; // Deactivate boss
            // Switch to game over screen or perform any other necessary actions
        }
    }
    */






    // Remove inactive enemies
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) {
        return !enemy.active;
    }), enemies.end());
    }
}



void updateBoss() {
    if (boss.active) {
        // Move boss horizontally (you can adjust this logic as needed)
        // Move boss horizontally
        if (bossMovingRight) {
            boss.x += 0.1f; // Move right
        } else {
            boss.x -= 0.1f; // Move left
        }

        // Check if boss reaches screen boundaries to change direction
        if (boss.x > 90.0f) {
            bossMovingRight = false; // Change direction to left
        } else if (boss.x < -90.0f) {
            bossMovingRight = true; // Change direction to right
        }

        //boss.x += 0.1f;

        // Check collision with bullets
        for (auto& bullet : bullets) {
            if (boss.active && bullet.second > boss.y + 10 && bullet.first > boss.x - 20 && bullet.first < boss.x + 20) {
                // Collision with bullet
                // Decrease boss health
                boss.health--;
                //bullet.second = 2.0f; // Move bullet out of screen
                //Move bullet out of the screen in the similar approach of Update enemy function
                bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                    [&](const std::pair<GLfloat, GLfloat>& b) {
                    return b.first == bullet.first && b.second == bullet.second;
                    }), bullets.end());

                if (boss.health <= 0) {
                    // Boss defeated, game won
                    //std::cout << "You defeated the boss! You win!" << std::endl;
                    //exit(0);
                    glutDisplayFunc(winnerScreen);
                    //winnerScreen();
                    glutPostRedisplay();
                    //break;
                }
            }
        }
    }
}


//Boss boss;
// Declare and initialize the boss variable

//boss.x = 0;
//boss.y = 90;
//boss.width = 20;
//boss.height = 20;

/*
void drawBoss() {

    //Boss boss;
    // Assuming boss is a Boss object
    float bossLeft = boss.x - boss.width / 2;
    float bossRight = boss.x + boss.width / 2;
    float bossTop = boss.y + boss.height / 2;
    float bossBottom = boss.y - boss.height / 2;
    GLfloat bossSize = 20;

    // Set boss color or texture
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for boss (you can change it based on your design)

    // Draw boss shape (for example, a square)
    glPushMatrix();
    glTranslatef(boss.x, boss.y, 0.0f); // Set boss position
    glBegin(GL_QUADS);
    glVertex2f(-bossSize / 2, -bossSize / 2); // Bottom-left
    glVertex2f(bossSize / 2, -bossSize / 2);  // Bottom-right
    glVertex2f(bossSize / 2, bossSize / 2);   // Top-right
    glVertex2f(-bossSize / 2, bossSize / 2);  // Top-left
    glEnd();
    glPopMatrix();
}
*/


void spaceShip()
{


    glPushMatrix();
    glTranslatef(xPosition,yPosition,0.0);

    //white background

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0, -22.0);
    glVertex2f(1.0, -22.0);
    glVertex2f(1.0, -45.0);
    glVertex2f(-1.0, -45.0);

    glEnd();


    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-2.0, -26.0);
    glVertex2f(2.0, -26.0);
    glVertex2f(2.0, -45.0);
    glVertex2f(-2.0, -45.0);

    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-3.0, -30.0);
    glVertex2f(3.0, -30.0);
    glVertex2f(3.0, -45.0);
    glVertex2f(-3.0, -45.0);

    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-4.0, -34.0);
    glVertex2f(4.0, -34.0);
    glVertex2f(4.0, -45.0);
    glVertex2f(-4.0, -45.0);

    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-5.0, -38.0);
    glVertex2f(5.0, -38.0);
    glVertex2f(5.0, -45.0);
    glVertex2f(-5.0, -45.0);

    glEnd();


    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-6.0, -42.0);
    glVertex2f(6.0, -42.0);
    glVertex2f(6.0, -45.0);
    glVertex2f(-6.0, -45.0);

    glEnd();


    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-7.0, -44.0);
    glVertex2f(7.0, -44.0);
    glVertex2f(7.0, -45.0);
    glVertex2f(-7.0, -45.0);

    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-8.0, -46.0);
    glVertex2f(8.0, -46.0);
    glVertex2f(8.0, -45.0);
    glVertex2f(-8.0, -45.0);

    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-9.0, -47.0);
    glVertex2f(-4.0, -47.0);
    glVertex2f(-4.0, -46.0);
    glVertex2f(-9.0, -46.0);

    glEnd();


    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-10.0, -48.0);
    glVertex2f(-6.0, -48.0);
    glVertex2f(-6.0, -47.0);
    glVertex2f(-10.0, -47.0);

    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-11.0, -49.0);
    glVertex2f(-8.0, -49.0);
    glVertex2f(-8.0, -48.0);
    glVertex2f(-11.0, -48.0);

    glEnd();



    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(9.0, -47.0);
    glVertex2f(4.0, -47.0);
    glVertex2f(4.0, -46.0);
    glVertex2f(9.0, -46.0);

    glEnd();


    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(10.0, -48.0);
    glVertex2f(6.0, -48.0);
    glVertex2f(6.0, -47.0);
    glVertex2f(10.0, -47.0);

    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(11.0, -49.0);
    glVertex2f(8.0, -49.0);
    glVertex2f(8.0, -48.0);
    glVertex2f(11.0, -48.0);

    glEnd();

    //side borders, all will be black

    //lower half of the spaceship

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-12.0, -50.0);
    glVertex2f(-8.0, -50.0);
    glVertex2f(-8.0, -49.0);
    glVertex2f(-12.0, -49.0);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(12.0, -50.0);
    glVertex2f(8.0, -50.0);
    glVertex2f(8.0, -49.0);
    glVertex2f(12.0, -49.0);

    glEnd();


    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-8.0, -49.0);
    glVertex2f(-6.0, -49.0);
    glVertex2f(-6.0, -48.0);
    glVertex2f(-8.0, -48.0);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-6.0, -48.0);
    glVertex2f(-4.0, -48.0);
    glVertex2f(-4.0, -47.0);
    glVertex2f(-6.0, -47.0);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-4.0, -47.0);
    glVertex2f(-2.0, -47.0);
    glVertex2f(-2.0, -46.0);
    glVertex2f(-4.0, -46.0);

    glEnd();


    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-2.0, -46.0);
    glVertex2f(2.0, -46.0);
    glVertex2f(2.0, -45.0);
    glVertex2f(-2.0, -45.0);

    glEnd();


    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(8.0, -49.0);
    glVertex2f(6.0, -49.0);
    glVertex2f(6.0, -48.0);
    glVertex2f(8.0, -48.0);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(6.0, -48.0);
    glVertex2f(4.0, -48.0);
    glVertex2f(4.0, -47.0);
    glVertex2f(6.0, -47.0);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(4.0, -47.0);
    glVertex2f(2.0, -47.0);
    glVertex2f(2.0, -46.0);
    glVertex2f(4.0, -46.0);

    glEnd();

    //upper half of the body on both side

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-12.0, -49.0);
    glVertex2f(-11.0, -49.0);
    glVertex2f(-11.0, -48.0);
    glVertex2f(-12.0, -48.0);

    glEnd();



    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-11.0, -48.0);
    glVertex2f(-10.0, -48.0);
    glVertex2f(-10.0, -47.0);
    glVertex2f(-11.0, -47.0);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-10.0, -47.0);
    glVertex2f(-9.0, -47.0);
    glVertex2f(-9.0, -46.0);
    glVertex2f(-10.0, -46.0);

    glEnd();


    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-9.0, -46.0);
    glVertex2f(-8.0, -46.0);
    glVertex2f(-8.0, -45.0);
    glVertex2f(-9.0, -45.0);

    glEnd();


    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-8.0, -45.0);
    glVertex2f(-7.0, -45.0);
    glVertex2f(-7.0, -44.0);
    glVertex2f(-8.0, -44.0);

    glEnd();


    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(12.0, -49.0);
    glVertex2f(11.0, -49.0);
    glVertex2f(11.0, -48.0);
    glVertex2f(12.0, -48.0);
    glEnd();



    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(11.0, -48.0);
    glVertex2f(10.0, -48.0);
    glVertex2f(10.0, -47.0);
    glVertex2f(11.0, -47.0);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(10.0, -47.0);
    glVertex2f(9.0, -47.0);
    glVertex2f(9.0, -46.0);
    glVertex2f(10.0, -46.0);

    glEnd();


    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(9.0, -46.0);
    glVertex2f(8.0, -46.0);
    glVertex2f(8.0, -45.0);
    glVertex2f(9.0, -45.0);

    glEnd();


    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(8.0, -45.0);
    glVertex2f(7.0, -45.0);
    glVertex2f(7.0, -44.0);
    glVertex2f(8.0, -44.0);

    glEnd();


    //upper half of the body

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-7.0, -44.0);
    glVertex2f(-6.0, -44.0);
    glVertex2f(-6.0, -42.0);
    glVertex2f(-7.0, -42.0);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-6.0, -42.0);
    glVertex2f(-5.0, -42.0);
    glVertex2f(-5.0, -38.0);
    glVertex2f(-6.0, -38.0);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-5.0, -38.0);
    glVertex2f(-4.0, -38.0);
    glVertex2f(-4.0, -34.0);
    glVertex2f(-5.0, -34.0);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-4.0, -34.0);
    glVertex2f(-3.0, -34.0);
    glVertex2f(-3.0, -30.0);
    glVertex2f(-4.0, -30.0);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-3.0, -30.0);
    glVertex2f(-2.0, -30.0);
    glVertex2f(-2.0, -26.0);
    glVertex2f(-3.0, -26.0);

    glEnd();


    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-2.0, -26.0);
    glVertex2f(-1.0, -26.0);
    glVertex2f(-1.0, -22.0);
    glVertex2f(-2.0, -22.0);

    glEnd();


    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0, -22.0);
    glVertex2f(1.0, -22.0);
    glVertex2f(1.0, -18.0);
    glVertex2f(-1.0, -18.0);

    glEnd();


    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(7.0, -44.0);
    glVertex2f(6.0, -44.0);
    glVertex2f(6.0, -42.0);
    glVertex2f(7.0, -42.0);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(6.0, -42.0);
    glVertex2f(5.0, -42.0);
    glVertex2f(5.0, -38.0);
    glVertex2f(6.0, -38.0);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(5.0, -38.0);
    glVertex2f(4.0, -38.0);
    glVertex2f(4.0, -34.0);
    glVertex2f(5.0, -34.0);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(4.0, -34.0);
    glVertex2f(3.0, -34.0);
    glVertex2f(3.0, -30.0);
    glVertex2f(4.0, -30.0);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(3.0, -30.0);
    glVertex2f(2.0, -30.0);
    glVertex2f(2.0, -26.0);
    glVertex2f(3.0, -26.0);

    glEnd();


    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(2.0, -26.0);
    glVertex2f(1.0, -26.0);
    glVertex2f(1.0, -22.0);
    glVertex2f(2.0, -22.0);

    glEnd();




    //blue core

    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0, -34.0);
    glVertex2f(1.0, -34.0);
    glVertex2f(1.0, -32.0);
    glVertex2f(-1.0, -32.0);

    glEnd();


    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-2.0, -38.0);
    glVertex2f(2.0, -38.0);
    glVertex2f(2.0, -34.0);
    glVertex2f(-2.0, -34.0);

    glEnd();


    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-3.0, -38.0);
    glVertex2f(3.0, -38.0);
    glVertex2f(3.0, -43.0);
    glVertex2f(-3.0, -43.0);

    glEnd();


    // red texture, lower half


    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-8.0, -48.0);
    glVertex2f(-7.0, -48.0);
    glVertex2f(-7.0, -47.0);
    glVertex2f(-8.0, -47.0);

    glEnd();


    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-7.0, -47.0);
    glVertex2f(-5.0, -47.0);
    glVertex2f(-5.0, -46.0);
    glVertex2f(-7.0, -46.0);

    glEnd();


    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-6.0, -46.0);
    glVertex2f(-4.0, -46.0);
    glVertex2f(-4.0, -45.0);
    glVertex2f(-6.0, -45.0);

    glEnd();



    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(8.0, -48.0);
    glVertex2f(7.0, -48.0);
    glVertex2f(7.0, -47.0);
    glVertex2f(8.0, -47.0);

    glEnd();


    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(7.0, -47.0);
    glVertex2f(5.0, -47.0);
    glVertex2f(5.0, -46.0);
    glVertex2f(7.0, -46.0);

    glEnd();


    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(6.0, -46.0);
    glVertex2f(4.0, -46.0);
    glVertex2f(4.0, -45.0);
    glVertex2f(6.0, -45.0);

    glEnd();



    //red texture upper half

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-3.0, -44.0);
    glVertex2f(3.0, -44.0);
    glVertex2f(3.0, -43.0);
    glVertex2f(-3.0, -43.0);

    glEnd();


    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0, -30.0);
    glVertex2f(1.0, -30.0);
    glVertex2f(1.0, -27.0);
    glVertex2f(-1.0, -27.0);

    glEnd();


    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0, -32.0);
    glVertex2f(1.0, -32.0);
    glVertex2f(1.0, -31.0);
    glVertex2f(-1.0, -31.0);

    glEnd();


    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-4.0, -43.0);
    glVertex2f(-3.0, -43.0);
    glVertex2f(-3.0, -38.0);
    glVertex2f(-4.0, -38.0);

    glEnd();



    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-3.0, -38.0);
    glVertex2f(-2.0, -38.0);
    glVertex2f(-2.0, -34.0);
    glVertex2f(-3.0, -34.0);

    glEnd();


    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-2.0, -34.0);
    glVertex2f(-1.0, -34.0);
    glVertex2f(-1.0, -32.0);
    glVertex2f(-2.0, -32.0);

    glEnd();


    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(4.0, -43.0);
    glVertex2f(3.0, -43.0);
    glVertex2f(3.0, -38.0);
    glVertex2f(4.0, -38.0);

    glEnd();



    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(3.0, -38.0);
    glVertex2f(2.0, -38.0);
    glVertex2f(2.0, -34.0);
    glVertex2f(3.0, -34.0);

    glEnd();


    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(2.0, -34.0);
    glVertex2f(1.0, -34.0);
    glVertex2f(1.0, -32.0);
    glVertex2f(2.0, -32.0);

    glEnd();
    glPopMatrix();


}


void drawBoss()
{

    glScalef(0.5,0.5,1);
    glPushMatrix();
//    glTranslatef(xPosition,yPosition,0.0);

//color_filler
 glColor3f(0.298f, 0.686f, 0.314f);
     glBegin(GL_QUADS);
    glVertex2f(12.0, -35.0);
    glVertex2f(-12.0, -35.0);
    glVertex2f(-12.0, -15.0);
    glVertex2f(12.0, -15.0);

    glEnd();
  //c2
    glColor3f(0.298f, 0.686f, 0.314f);
     glBegin(GL_QUADS);
    glVertex2f(8.0, 10.0);
    glVertex2f(-8.0, 10.0);
    glVertex2f(-8.0, 43.0);
    glVertex2f(8.0, 43.0);

    glEnd();

//c1
glColor3f(0.105f, 0.368f, 0.125f);
     glBegin(GL_QUADS);
    glVertex2f(5.0, 25.0);
    glVertex2f(-5.0, 25.0);
    glVertex2f(-5.0, 43.0);
    glVertex2f(5.0, 43.0);

    glEnd();

    //c3
       glColor3f(0.105f, 0.368f, 0.125f);
     glBegin(GL_QUADS);
    glVertex2f(8.0, 45.0);
    glVertex2f(8.0, 5.0);
    glVertex2f(17.0, 5.0);
    glVertex2f(17.0, 45.0);

    glEnd();
    //c3
       glColor3f(0.105f, 0.368f, 0.125f);
     glBegin(GL_QUADS);
    glVertex2f(-8.0, 45.0);
    glVertex2f(-8.0, 5.0);
    glVertex2f(-17.0, 5.0);
    glVertex2f(-17.0, 45.0);

    glEnd();
    //c4
      glColor3f(0.105f, 0.368f, 0.125f);
     glBegin(GL_QUADS);
    glVertex2f(23.0, 17.0);
    glVertex2f(23.0, 5.0);
    glVertex2f(17.0, 5.0);
    glVertex2f(17.0, 17.0);

    glEnd();
     //c4
      glColor3f(0.105f, 0.368f, 0.125f);
     glBegin(GL_QUADS);
    glVertex2f(-23.0, 17.0);
    glVertex2f(-23.0, 5.0);
    glVertex2f(-17.0, 5.0);
    glVertex2f(-17.0, 17.0);

    glEnd();
    //c5
      glColor3f(0.698f, 0.878f, 0.845f);
     glBegin(GL_QUADS);
    glVertex2f(20.0, 41.0);
    glVertex2f(20.0, 17.0);
    glVertex2f(15.0, 17.0);
    glVertex2f(15.0, 41.0);

    glEnd();
    //c5
      glColor3f(0.698f, 0.878f, 0.845f);
     glBegin(GL_QUADS);
    glVertex2f(-20.0, 41.0);
    glVertex2f(-20.0, 17.0);
    glVertex2f(-15.0, 17.0);
    glVertex2f(-15.0, 41.0);

    glEnd();
    //c6
    glColor3f(0.105f, 0.368f, 0.125f);
     glBegin(GL_QUADS);
    glVertex2f(23.0, 43.0);
    glVertex2f(36.0, 43.0);
    glVertex2f(36.0, 40.0);
    glVertex2f(23.0, 25.0);

    glEnd();
    glColor3f(0.105f, 0.368f, 0.125f);
     glBegin(GL_QUADS);
    glVertex2f(-23.0, 43.0);
    glVertex2f(-36.0, 43.0);
    glVertex2f(-36.0, 40.0);
    glVertex2f(-23.0, 25.0);

    glEnd();


     //c7
   glColor3f(0.298f, 0.686f, 0.314f);
     glBegin(GL_POLYGON);
    glVertex2f(21.0, 10.0);
    glVertex2f(30.0, 12.0);
    glVertex2f(40.0, 14.0);
    glVertex2f(50.0, 14.0);
    glVertex2f(50.0, 10.0);
    glVertex2f(21.0, -12.0);
    glVertex2f(19.0, -10.0);

    glEnd();
        glColor3f(0.298f, 0.686f, 0.314f);
     glBegin(GL_POLYGON);
    glVertex2f(-21.0, 10.0);
    glVertex2f(-30.0, 12.0);
    glVertex2f(-40.0, 14.0);
    glVertex2f(-50.0, 14.0);
    glVertex2f(-50.0, 10.0);
    glVertex2f(-21.0, -12.0);
    glVertex2f(-19.0, -10.0);

    glEnd();


       //c8
   glColor3f(0.0f, 0.1f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(10.0, 3.0);
    glVertex2f(19.0, 3.0);
    glVertex2f(19.0, -20.0);
    glVertex2f(10.0, -20.0);

    glEnd();
     glColor3f(0.0f, 0.1f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-10.0, 3.0);
    glVertex2f(-19.0, 3.0);
    glVertex2f(-19.0, -20.0);
    glVertex2f(-10.0, -20.0);

    glEnd();

    //C9
      glColor3f(0.0f, 0.1f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(10.0, -25.0);
    glVertex2f(15.0, -25.0);
    glVertex2f(15.0, -20.0);
    glVertex2f(10.0, -20.0);

    glEnd();
      glColor3f(0.0f, 0.1f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-10.0, -25.0);
    glVertex2f(-15.0, -25.0);
    glVertex2f(-15.0, -20.0);
    glVertex2f(-10.0, -20.0);

    glEnd();

     //C10
      glColor3f(0.0f, 1.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(8.0, 10.0);
    glVertex2f(-8.0, 10.0);
    glVertex2f(-8.0, -14.0);
    glVertex2f(8.0, -14.0);

    glEnd();
   //C10
    glColor3f(0.698f, 0.878f, 0.845f);
     glBegin(GL_QUADS);
    glVertex2f(8.0, -22.0);
    glVertex2f(-8.0, -22.0);
    glVertex2f(-8.0, -15.0);
    glVertex2f(8.0, -15.0);

    glEnd(); //C11
    glColor3f(0.698f, 0.878f, 0.845f);
     glBegin(GL_POLYGON);
    glVertex2f(9.0, -22.0);
    glVertex2f(0.0, -36.0);
    glVertex2f(-9.0, -22.0);


    glEnd();
    //C12
    glColor3f(0.298f, 0.686f, 0.314f);
     glBegin(GL_POLYGON);
    glVertex2f(12.0, -35.0);
    glVertex2f(0.0, -50.0);
    glVertex2f(-12.0, -35.0);


    glEnd();


     //GUN
        glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(40.0,4.0);
     glVertex2f(40.0,-6.0);
    glVertex2f(36.0,-6.0);
    glVertex2f(36.0,2.0);

    glEnd();


     //GUN
      glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-40.0,4.0);
     glVertex2f(-40.0,-6.0);
    glVertex2f(-36.0,-6.0);
    glVertex2f(-36.0,2.0);

    glEnd();

     //GUNIN
        glColor3f(0.901f, 0.675f, 0.182f);
    glBegin(GL_QUADS);
    glVertex2f(39.0,4.0);
     glVertex2f(39.0,-4.0);
    glVertex2f(37.0,-4.0);
    glVertex2f(37.0,2.0);

    glEnd();
     //GUNIN
        glColor3f(0.901f, 0.675f, 0.182f);
    glBegin(GL_QUADS);
    glVertex2f(-39.0,4.0);
     glVertex2f(-39.0,-4.0);
    glVertex2f(-37.0,-4.0);
    glVertex2f(-37.0,2.0);

    glEnd();












//tail
//1
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(0.0, 45.0);
    glVertex2f(0.0, 43.0);
    glVertex2f(5.0, 43.0);
    glVertex2f(5.0, 45.0);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(0.0, 45.0);
    glVertex2f(0.0, 43.0);
    glVertex2f(-5.0, 43.0);
    glVertex2f(-5.0, 45.0);

    glEnd();
//2
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(3.0, 43.0);
    glVertex2f(5.0, 43.0);
    glVertex2f(5.0, 25.0);
    glVertex2f(3.0, 25.0);

    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-3.0, 43.0);
    glVertex2f(-5.0, 43.0);
    glVertex2f(-5.0, 25.0);
    glVertex2f(-3.0, 25.0);

    glEnd();
     //3
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(0.0, 25.0);
     glVertex2f(3.0, 25.0);
    glVertex2f(3.0, 23.0);

    glVertex2f(0.0, 23.0);


    glEnd();
     glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-0.0, 25.0);
     glVertex2f(-3.0, 25.0);
    glVertex2f(-3.0, 23.0);

    glVertex2f(-0.0, 23.0);


    glEnd();
  //4
  glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(5.0, 43.0);
    glVertex2f(8.0, 43.0);
    glVertex2f(8.0, 41.0);
    glVertex2f(5.0, 41.0);

    glEnd();
     glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-5.0, 43.0);
    glVertex2f(-8.0, 43.0);
    glVertex2f(-8.0, 41.0);
    glVertex2f(-5.0, 41.0);

    glEnd();
    //5
     glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(8.0, 45.0);
    glVertex2f(10.0, 45.0);
    glVertex2f(10.0, 15.0);
    glVertex2f(8.0, 15.0);

    glEnd();
     glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-8.0, 45.0);
    glVertex2f(-10.0, 45.0);
    glVertex2f(-10.0, 15.0);
    glVertex2f(-8.0, 15.0);

    glEnd();
    //6
     glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(10.0, 45.0);
    glVertex2f(10.0, 43.0);
    glVertex2f(17.0, 43.0);
    glVertex2f(17.0, 45.0);

    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-10.0, 45.0);
    glVertex2f(-10.0, 43.0);
    glVertex2f(-17.0, 43.0);
    glVertex2f(-17.0, 45.0);

    glEnd();
    //7
  glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(19.0, 43.0);
    glVertex2f(19.0, 41.0);
    glVertex2f(17.0, 41.0);
    glVertex2f(17.0, 43.0);

    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-19.0, 43.0);
    glVertex2f(-19.0, 41.0);
    glVertex2f(-17.0, 41.0);
    glVertex2f(-17.0, 43.0);

    glEnd();

    //8
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(15.0, 43.0);
    glVertex2f(17.0, 43.0);
    glVertex2f(17.0, 17.0);
    glVertex2f(15.0, 17.0);

    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-15.0, 43.0);
    glVertex2f(-17.0, 43.0);
    glVertex2f(-17.0, 17.0);
    glVertex2f(-15.0, 17.0);

    glEnd();
    //9
     glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(19.0, 43.0);
    glVertex2f(21.0, 43.0);
    glVertex2f(21.0, 17.0);
    glVertex2f(19.0, 17.0);

    glEnd();
     glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-19.0, 43.0);
    glVertex2f(-21.0, 43.0);
    glVertex2f(-21.0, 17.0);
    glVertex2f(-19.0, 17.0);

    glEnd();
    //10
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(21.0, 41.0);
    glVertex2f(23.0, 41.0);
    glVertex2f(23.0, 5.0);
    glVertex2f(21.0, 5.0);

    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-21.0, 41.0);
    glVertex2f(-23.0, 41.0);
    glVertex2f(-23.0, 5.0);
    glVertex2f(-21.0, 5.0);

    glEnd();

    //11
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(8.0, 3.0);
    glVertex2f(21.0, 3.0);
    glVertex2f(21.0, 5.0);
    glVertex2f(8.0, 5.0);

    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-8.0, 3.0);
    glVertex2f(-21.0, 3.0);
    glVertex2f(-21.0, 5.0);
    glVertex2f(-8.0, 5.0);

    glEnd();
    //12
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(6.0, 5.0);
    glVertex2f(6.0, 15.0);
    glVertex2f(8.0, 15.0);
    glVertex2f(8.0, 5.0);

    glEnd();
     glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-6.0, 5.0);
    glVertex2f(-6.0, 15.0);
    glVertex2f(-8.0, 15.0);
    glVertex2f(-8.0, 5.0);

    glEnd();
    //13
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(6.0, 10.0);
    glVertex2f(6.0, 12.0);
    glVertex2f(3.0, 12.0);
    glVertex2f(3.0, 10.0);

    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-6.0, 10.0);
    glVertex2f(-6.0, 12.0);
    glVertex2f(-3.0, 12.0);
    glVertex2f(-3.0, 10.0);

    glEnd();
    //14
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(0.0, 10.0);
    glVertex2f(0.0, 8.0);
    glVertex2f(-3.0, 8.0);
    glVertex2f(-3.0, 10.0);

    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(0.0, 10.0);
    glVertex2f(0.0, 8.0);
    glVertex2f(3.0, 8.0);
    glVertex2f(3.0, 10.0);

    glEnd();

    //15
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(19.0, 17.0);
    glVertex2f(19.0, 15.0);
    glVertex2f(17.0, 15.0);
    glVertex2f(17.0, 17.0);

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-19.0, 17.0);
    glVertex2f(-19.0, 15.0);
    glVertex2f(-17.0, 15.0);
    glVertex2f(-17.0, 17.0);

    glEnd();
    //16
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(23.0, 41.0);
    glVertex2f(23.0,43.0);
    glVertex2f(27.0,43.0);
    glVertex2f(27.0,41.0);

    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-23.0, 41.0);
    glVertex2f(-23.0,43.0);
    glVertex2f(-27.0,43.0);
    glVertex2f(-27.0,41.0);

    glEnd();
    //17
     glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(35.0,45.0);
    glVertex2f(35.0,43.0);
    glVertex2f(27.0,43.0);
    glVertex2f(27.0,45.0);
       glEnd();
       glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-35.0,45.0);
    glVertex2f(-35.0,43.0);
    glVertex2f(-27.0,43.0);
    glVertex2f(-27.0,45.0);
       glEnd();

        //18
     glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(35.0,45.0);
    glVertex2f(35.0,39.0);
    glVertex2f(37.0,39.0);
    glVertex2f(37.0,45.0);
       glEnd();
        glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-35.0,45.0);
    glVertex2f(-35.0,39.0);
    glVertex2f(-37.0,39.0);
    glVertex2f(-37.0,45.0);
       glEnd();

       //19
       glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(35.0,41.0);
    glVertex2f(39.0,39.0);
    glVertex2f(23.0,26.0);
    glVertex2f(21.0,22.0);
       glEnd();
        glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-35.0,41.0);
    glVertex2f(-39.0,39.0);
    glVertex2f(-23.0,26.0);
    glVertex2f(-21.0,22.0);
       glEnd();
      //20
       glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(10.0,3.0);
    glVertex2f(8.0,3.0);
    glVertex2f(8.0,-25.0);
    glVertex2f(10.0,-25.0);
       glEnd();


    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(-10.0,3.0);
    glVertex2f(-8.0,3.0);
    glVertex2f(-8.0,-25.0);
    glVertex2f(-10.0,-25.0);
       glEnd();
//21
     glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(21.0,3.0);
    glVertex2f(19.0,3.0);
    glVertex2f(19.0,-15.0);
    glVertex2f(21.0,-15.0);
       glEnd();

        glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-21.0,3.0);
    glVertex2f(-19.0,3.0);
    glVertex2f(-19.0,-15.0);
    glVertex2f(-21.0,-15.0);
       glEnd();

       //22
         glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(3.0,-17.0);
    glVertex2f(8.0,-17.0);
    glVertex2f(8.0,-15.0);
    glVertex2f(3.0,-15.0);
       glEnd();
          glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_QUADS);
    glVertex2f(-3.0,-17.0);
    glVertex2f(-8.0,-17.0);
    glVertex2f(-8.0,-15.0);
    glVertex2f(-3.0,-15.0);
       glEnd();
//23
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(3.0,-12.0);
    glVertex2f(3.0,-15.0);
    glVertex2f(-3.0,-15.0);
    glVertex2f(-3.0,-12.0);
       glEnd();
       //24
      glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(8.0,-20.0);
    glVertex2f(8.0,-23.0);
    glVertex2f(-8.0,-23.0);
    glVertex2f(-8.0,-20.0);
       glEnd();
       //25
         glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(0.0,-35.0);
    glVertex2f(0.0,-38.0);
    glVertex2f(9.0,-25.0);
    glVertex2f(9.0,-22.0);
       glEnd();
        glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(0.0,-35.0);
    glVertex2f(0.0,-38.0);
    glVertex2f(-9.0,-25.0);
    glVertex2f(-9.0,-22.0);
       glEnd();
       //26
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(19.0,-15.0);
    glVertex2f(16.0,-15.0);
    glVertex2f(16.0,-20.0);
    glVertex2f(19.0,-20.0);
       glEnd();
       glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-19.0,-15.0);
    glVertex2f(-16.0,-15.0);
    glVertex2f(-16.0,-20.0);
    glVertex2f(-19.0,-20.0);
       glEnd();
//27
         glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(13.0,-20.0);
      glVertex2f(16.0,-20.0);
    glVertex2f(16.0,-25.0);
    glVertex2f(13.0,-25.0);
       glEnd();
           glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-13.0,-20.0);
    glVertex2f(-16.0,-20.0);
    glVertex2f(-16.0,-25.0);

    glVertex2f(-13.0,-25.0);
       glEnd();
       //28
      glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(13.0,-35.0);
     glVertex2f(10.0,-35.0);
    glVertex2f(10.0,-25.0);
    glVertex2f(13.0,-25.0);
       glEnd();

        glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-13.0,-35.0);
    glVertex2f(-10.0,-35.0);
    glVertex2f(-10.0,-25.0);
    glVertex2f(-13.0,-25.0);
       glEnd();
    glEnd();
    //29
   glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(13.0,-35.0);
     glVertex2f(10.0,-35.0);
    glVertex2f(0.0,-50.0);
    glVertex2f(3.0,-50.0);
       glEnd();
       glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-13.0,-35.0);
     glVertex2f(-10.0,-35.0);
    glVertex2f(-0.0,-50.0);
    glVertex2f(-3.0,-50.0);
       glEnd();
       //30
       glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(3.0,-50.0);
     glVertex2f(-3.0,-50.0);
    glVertex2f(-1.0,-54.0);
    glVertex2f(1.0,-54.0);
       glEnd();
       //31
       glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(23.0,10.0);
     glVertex2f(30.0,10.0);
    glVertex2f(30.0,12.0);
    glVertex2f(23.0,12.0);

    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-23.0,10.0);
     glVertex2f(-30.0,10.0);
    glVertex2f(-30.0,12.0);
    glVertex2f(-23.0,12.0);

    glEnd();
    //32
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(40.0,12.0);
     glVertex2f(30.0,12.0);
    glVertex2f(30.0,14.0);
    glVertex2f(40.0,14.0);

    glEnd();
     glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-40.0,12.0);
     glVertex2f(-30.0,12.0);
    glVertex2f(-30.0,14.0);
    glVertex2f(-40.0,14.0);

    glEnd();
    //33
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(40.0,14.0);
     glVertex2f(40.0,16.0);
    glVertex2f(50.0,16.0);
    glVertex2f(50.0,14.0);

    glEnd();
     glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-40.0,14.0);
     glVertex2f(-40.0,16.0);
    glVertex2f(-50.0,16.0);
    glVertex2f(-50.0,14.0);

    glEnd();
    //34
      glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(50.0,20.0);
     glVertex2f(54.0,20.0);
    glVertex2f(50.0,10.0);
    glVertex2f(50.0,10.0);

    glEnd();
      glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-50.0,20.0);
     glVertex2f(-54.0,20.0);
    glVertex2f(-50.0,10.0);
    glVertex2f(-50.0,10.0);

    glEnd();
     //35
       glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(50.0,12.0);
     glVertex2f(52.0,10.0);
    glVertex2f(21.0,-12.0);
    glVertex2f(19.0,-10.0);

    glEnd();
        glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-50.0,12.0);
     glVertex2f(-52.0,10.0);
    glVertex2f(-21.0,-12.0);
    glVertex2f(-19.0,-10.0);

    glEnd();



    glEnd();
}




void drawEnemy()
{

    //int x=0;
    //int y=0;
    glScalef(0.5,0.5,1);
    glPushMatrix();
//    glTranslatef(xPosition,yPosition,0.0);


 //filler
 //HEAD

    glColor3f(1.0f, 1.0f, 1.0f);
     glBegin(GL_POLYGON);
    glVertex2f(0, -19.0);
    glVertex2f(4.3, -10.5);
    glVertex2f(-4.3, -10.5);

    glEnd();
  glColor3f(0.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(0, -16.0);
    glVertex2f(2, -12);
    glVertex2f(0, 13.5);
    glVertex2f(-2, -12);

    glEnd();

    //MIDDLE

 glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(0.0, -10.0);
    glVertex2f(2.0, -10.0);
    glVertex2f(10.0, 0.0);
    glVertex2f(0.0, -5.0);

    glEnd();

 glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(0.0, -10.0);
    glVertex2f(-2.0, -10.0);
    glVertex2f(-10.0, 0.0);
    glVertex2f(0.0, -5.0);

    glEnd();


    //BODY
      glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);

    glVertex2f(0.0, -5.0);
    glVertex2f(6.0, -1.0);
    glVertex2f(5.0, 5.0);
    glVertex2f(0.0, 5.0);

    glEnd();

      glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);

    glVertex2f(0.0, -5.0);
    glVertex2f(-6.0, -1.0);
    glVertex2f(-5.0, 5.0);
    glVertex2f(0.0, 5.0);

    glEnd();

    //SIDEWING
glColor3f(0.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);

    glVertex2f(13.5, -3.0);
    glVertex2f(14.9, 4.5);
    glVertex2f(8.5, 8.0);


    glEnd();
glColor3f(0.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);

    glVertex2f(-13.5, -3.0);
    glVertex2f(-14.9, 4.5);
    glVertex2f(-8.5, 8.0);


    glEnd();

    //TAIL

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);

    glVertex2f(0.0, 5.0);
    glVertex2f(2.0, 5.0);
    glVertex2f(3.5, 10.0);
     glVertex2f(0.0, 10.0);

    glEnd();
   glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);

    glVertex2f(0.0, 5.0);
    glVertex2f(-2.0, 5.0);
    glVertex2f(-3.5, 10.0);
     glVertex2f(0.0, 10.0);

    glEnd();

    //middleorg

  /*glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(0, -16.0);
    glVertex2f(2, -12);
    glVertex2f(0, 13.5);
    glVertex2f(-2, -12);

    glEnd();*/

//headblue
      glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(0, -16.0);
    glVertex2f(2, -12);

    glVertex2f(-2, -12);

    glEnd();





//head
 glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(0.0, -20.0);
    glVertex2f(0.0, -19.0);
    glVertex2f(4.0, -10.0);
    glVertex2f(5.0, -10.0);

    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(0.0, -20.0);
    glVertex2f(0.0, -19.0);
    glVertex2f(-4.0, -10.0);
    glVertex2f(-5.0, -10.0);

    glEnd();

     glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(-5.0, -10.0);
    glVertex2f(-4.0, -11.0);
    glVertex2f(4.0, -11.0);
    glVertex2f(5.0, -10.0);

    glEnd();
    //middle

 glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(2.0, -10.0);
    glVertex2f(1.0, -10.0);
    glVertex2f(9.0, 0.0);
    glVertex2f(10.0, 0.0);

    glEnd();

      glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(-2.0, -10.0);
    glVertex2f(-1.0, -10.0);
    glVertex2f(-9.0, 0.0);
    glVertex2f(-10.0, 0.0);

    glEnd();

glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(9.0, -1.0);
    glVertex2f(9.0, 0.0);
    glVertex2f(0.0, -4.0);
    glVertex2f(0.0, -5.0);

    glEnd();

glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(-9.0, -1.0);
    glVertex2f(-9.0, 0.0);
    glVertex2f(0.0, -4.0);
    glVertex2f(0.0, -5.0);

    glEnd();
//body
glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(6.0, -1.0);
    glVertex2f(5.0, -2.0);
    glVertex2f(4.0, 5.0);
    glVertex2f(5.0, 5.0);

    glEnd();
glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-6.0, -1.0);
    glVertex2f(-5.0, -2.0);
    glVertex2f(-4.0, 5.0);
    glVertex2f(-5.0, 5.0);

    glEnd();

 glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(5.0, 5.0);
    glVertex2f(-5.0, 5.0);
    glVertex2f(-5.2, 4.0);
    glVertex2f(5.2, 4.0);

    glEnd();
    //wing
glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(5.0, 5.0);
    glVertex2f(10.0, 5.0);
    glVertex2f(11.5, 2.0);
    glVertex2f(5.2, 3.0);

    glEnd();
glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-5.0, 5.0);
    glVertex2f(-10.0, 5.0);
    glVertex2f(-11.5, 2.0);
    glVertex2f(-5.2, 3.0);

    glEnd();

    //sidewing

  glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(13.0, -3.0);
    glVertex2f(14.0, -3.0);
    glVertex2f(8.0, 8.0);
    glVertex2f(9.0, 8.0);

    glEnd();

glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(13.0, -3.0);
    glVertex2f(14.0, -3.0);
    glVertex2f(16.0, 5.0);
    glVertex2f(15.0, 5.0);



    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(8.0, 8.0);
    glVertex2f(15.0, 5.0);
    glVertex2f(15.0, 4.0);
    glVertex2f(9.0, 7.0);

    glEnd();

 glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-13.0, -3.0);
    glVertex2f(-14.0, -3.0);
    glVertex2f(-8.0, 8.0);
    glVertex2f(-9.0, 8.0);

    glEnd();


 glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-13.0, -3.0);
    glVertex2f(-14.0, -3.0);
    glVertex2f(-16.0, 5.0);
    glVertex2f(-15.0, 5.0);



    glEnd();

   glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-8.0, 8.0);
    glVertex2f(-15.0, 5.0);
    glVertex2f(-15.0, 4.0);
    glVertex2f(-9.0, 7.0);

    glEnd();

    //tail

   glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(2.0, 5.0);
    glVertex2f(2.5, 5.0);
    glVertex2f(3.5, 10.0);
    glVertex2f(2.5, 10.0);

    glEnd();
     glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-2.0, 5.0);
    glVertex2f(-2.5, 5.0);
    glVertex2f(-3.5, 10.0);
    glVertex2f(-2.5, 10.0);

    glEnd();

    glEnd();

     glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(0.0, 9.0);
    glVertex2f(0.0, 10.0);
    glVertex2f(3.5, 10.0);
    glVertex2f(3.5, 11.0);



    glEnd();


    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(0.0, 9.0);
    glVertex2f(0.0, 10.0);
    glVertex2f(-3.5, 10.0);
    glVertex2f(-3.5, 11.0);



    glEnd();


    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POLYGON);

    glVertex2f(0.0, 4.0);
    glVertex2f(3.0, 0.0);
    glVertex2f(-3.0, 0.0);




    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POLYGON);

    glVertex2f(0.0, -2.0);
    glVertex2f(3.0, 2.0);
    glVertex2f(-3.0, 2.0);





    glEnd();






    glEnd();



}




void drawBullets() {
    glColor3f(199.0f/255, 21.0f/255, 133.0f/255); // Blue color for bullets
    for (const auto& bullet : bullets) {
        glPushMatrix(); // Save current matrix
        glTranslatef(bullet.first, bullet.second, 0.0f);


        drawFilledCircle(0.0,-24.0,0.5,1000);
        drawFilledCircle(0.0,-22.0,0.5,1000);
        //glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(-0.5, -24.0);
        glVertex2f(0.5, -24.0);
        glVertex2f(0.5, -22.0);
        glVertex2f(-0.5, -22.0);
        glEnd();


        glPopMatrix(); // Restore previous matrix
    }
}

void information()
{
    //gluOrtho2D(-1,1,-1,1);
    //glClearColor(0.1294f, 0.1294f, 0.1294f, 1.0f); // Set background color to dark grey
    glClearColor(0.0, 0.0, 0.0, 1.0f); // Set background color to dark grey
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer (background)
    glLoadIdentity();

    glColor3f(1.0f, 0.0f, 0.0f); // Set text color to orange


    // Render the title with a different font (GLUT_BITMAP_HELVETICA_18)
    renderBitmapString(-40, 90, 0.0, GLUT_BITMAP_HELVETICA_18, "SPACE SENTINELS - ASTEROID ANNIHILATION CHRONICLES");


    // Render the subtitle with a different font (GLUT_BITMAP_8_BY_13)
    renderBitmapString(-15, 80, 0.0, GLUT_BITMAP_8_BY_13, "Computer Graphics (Section G)");

    // Render the group members with a different font (GLUT_BITMAP_TIMES_ROMAN_24)
    renderBitmapString(-90, 70, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, "Group Members:");

    glColor3f(1.0f, 1.0f, 1.0f); // Set text color to white

    glBegin(GL_LINES);
    glVertex2f(-100, 65);
    glVertex2f(100, 65);
    glEnd();

    renderBitmapString(-85, 60, 0.0f, GLUT_BITMAP_HELVETICA_12, "ID");
    renderBitmapString(-20, 60, 0.0f, GLUT_BITMAP_HELVETICA_12, "Name");
    renderBitmapString(45, 60, 0.0f, GLUT_BITMAP_HELVETICA_12, "Contribution");

    renderBitmapString(-90, 50, 0.0f, GLUT_BITMAP_HELVETICA_18, "22-48697-3");
    renderBitmapString(-30, 50, 0.0f, GLUT_BITMAP_HELVETICA_18, "ASHIQUR RAHMAN SARON");
    renderBitmapString(50, 50, 0.0f, GLUT_BITMAP_HELVETICA_18, "20");

    renderBitmapString(-90, 40, 0.0f, GLUT_BITMAP_HELVETICA_18, "22-48679-3");
    renderBitmapString(-30, 40, 0.0f, GLUT_BITMAP_HELVETICA_18, "HASIN ALMAS SIFAT");
    renderBitmapString(50, 40, 0.0f, GLUT_BITMAP_HELVETICA_18, "20");

    renderBitmapString(-90, 30, 0.0f, GLUT_BITMAP_HELVETICA_18, "22-46495-1");
    renderBitmapString(-30, 30, 0.0f, GLUT_BITMAP_HELVETICA_18, "MAHMUDA AKTER MUNNI");
    renderBitmapString(50, 30, 0.0f, GLUT_BITMAP_HELVETICA_18, "20");

    renderBitmapString(-90, 20, 0.0f, GLUT_BITMAP_HELVETICA_18, "21-45289-2");
    renderBitmapString(-30, 20, 0.0f, GLUT_BITMAP_HELVETICA_18, "NABILA TABASSUM BORSHA");
    renderBitmapString(50, 20, 0.0f, GLUT_BITMAP_HELVETICA_18, "20");

    renderBitmapString(-90, 10, 0.0f, GLUT_BITMAP_HELVETICA_18, "21-44853-2 ");
    renderBitmapString(-30, 10, 0.0f, GLUT_BITMAP_HELVETICA_18, "AMIT HASAN RUHIN");
    renderBitmapString(50, 10, 0.0f, GLUT_BITMAP_HELVETICA_18, "20");

    glBegin(GL_LINES);
    glVertex2f(-100, 5);
    glVertex2f(100, 5);
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f); // Set text color to green

    // Render the supervisor information with a different font (GLUT_BITMAP_9_BY_15)
    renderBitmapString(-7.0, -5, 0.0f, GLUT_BITMAP_9_BY_15, "Supervised by:");
    renderBitmapString(-15, -15, 0.0f, GLUT_BITMAP_9_BY_15, "Mahfujur Rahman (Lecturer)");
    renderBitmapString(-18.0, -25, 0.0f, GLUT_BITMAP_9_BY_15, "Department of Computer Science");
    renderBitmapString(-20.3, -35, 0.0f, GLUT_BITMAP_9_BY_15, "Faculty of Science and Technology");
    renderBitmapString(-32.0, -45, 0.0f, GLUT_BITMAP_9_BY_15, "American International University-Bangladesh (AIUB)");
    glColor3f(1.0f, 0.5f, 0.0f);
    renderBitmapString(-15.0, -80, 0.0f, GLUT_BITMAP_9_BY_15, "Press ESCAPE key to Enter Game");

    glFlush();
    glutSwapBuffers();
}




void designPattern();
void monitor();
void window();
void light();
void cockpit(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();


    //top most rectangle
    glColor3f(57.0f/255, 92.0f/255, 114.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-100.0, 100.0);
    glVertex2f(100.0, 100.0);
    glVertex2f(100.0, 88.0);
    glVertex2f(-100.0, 88.0);

    glEnd();


    //2nd rectangle from top
    glColor3f(57.0f/255, 92.0f/255, 114.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-100.0, 86.0);
    glVertex2f(100.0, 86.0);
    glVertex2f(100.0, -70.0);
    glVertex2f(-100.0, -70.0);

    glEnd();


    glColor3f(60.0f/255, 95.0f/255, 117.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-100.0, -70.0);
    glVertex2f(100.0, -70.0);
    glVertex2f(100.0, -71.0);
    glVertex2f(-100.0, -71.0);

    glEnd();


    glColor3f(55.0f/255, 87.0f/255, 108.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-100.0, -71.0);
    glVertex2f(100.0, -71.0);
    glVertex2f(100.0, -72.0);
    glVertex2f(-100.0, -72.0);

    glEnd();

    glColor3f(41.0f/255, 73.0f/255, 94.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-100.0, -72.0);
    glVertex2f(100.0, -72.0);
    glVertex2f(100.0, -74.0);
    glVertex2f(-100.0, -74.0);

    glEnd();

    glColor3f(31.0f/255, 64.0f/255, 83.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-100.0, -74.0);
    glVertex2f(100.0, -74.0);
    glVertex2f(100.0, -75.0);
    glVertex2f(-100.0, -75.0);

    glColor3f(41.0f/255, 79.0f/255, 98.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-100.0, -75.0);
    glVertex2f(100.0, -75.0);
    glVertex2f(100.0, -76.0);
    glVertex2f(-100.0, -76.0);

    glEnd();

    //2nd from the bottom rectangle

    glColor3f(88.0f/255, 132.0f/255, 141.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-100.0, -76.0);
    glVertex2f(100.0, -76.0);
    glVertex2f(100.0, -86.0);
    glVertex2f(-100.0, -86.0);

    glEnd();

    //bottom rectangle

    glColor3f(111.0f/255, 158.0f/255, 164.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-100.0, -86.0);
    glVertex2f(100.0, -86.0);
    glVertex2f(100.0, -100.0);
    glVertex2f(-100.0, -100.0);



    glEnd();

    designPattern();


    //1st bar
    glColor3f(36.0f/255, 65.0f/255, 83.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-100.0, 86.0);
    glVertex2f(100.0, 86.0);
    glVertex2f(100.0, 88.0);
    glVertex2f(-100.0, 88.0);


    //2nd bar
    glColor3f(36.0f/255, 65.0f/255, 83.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-100.0, 76.0);
    glVertex2f(100.0, 76.0);
    glVertex2f(100.0, 80.0);
    glVertex2f(-100.0, 80.0);

    glEnd();


    window();
    light();

    glPushMatrix();
    glTranslatef(-30.0,0.0f,1.0f);
    light();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(30.0,0.0f,1.0f);
    light();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(60.0f,0.0f,1.0f);
    light();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(90.0f,0.0f,1.0f);
    light();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(120.0f,0.0f,1.0f);
    light();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(150.f,0.0f,1.0f);
    light();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(180.0f,0.0f,1.0f);
    light();
    glPopMatrix();







    monitor();
    glPushMatrix();
    glTranslatef(140.0f,0.0f,1.0f);
    monitor();
    glPopMatrix();








    glEnd();

    //glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // Set background color to black and opaque
    //glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer (background)


    //renderBitmapString(-40, 20, 0.0, GLUT_BITMAP_TIMES_ROMAN_10, "START GAME");
    //renderBitmapString(-40, 10, 0.0, GLUT_BITMAP_TIMES_ROMAN_10, "");



    glFlush();
    glutSwapBuffers();

}

void designPattern()
{
    //design patterns left side
    glColor3f(192.0f/255, 192.0f/255, 192.0f/255);
    glLineWidth(3.0);
    glPushMatrix();
    glPointSize(5);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-46.0, 100.0);
    glVertex2f(-46.0, 70.0);
    glVertex2f(-48.0, 68.0);
    glVertex2f(-100.0, 68.0);
    glEnd();
    glPopMatrix();

    glColor3f(192.0f/255, 192.0f/255, 192.0f/255);
    glLineWidth(3.0);
    glPushMatrix();
    glBegin(GL_LINE_STRIP);
    glVertex2f(-44.0, 100.0);
    glVertex2f(-44.0, 68.0);
    glVertex2f(-46.0, 66.0);
    glVertex2f(-100.0, 66.0);
    glEnd();
    glPopMatrix();

    glColor3f(192.0f/255, 192.0f/255, 192.0f/255);
    glLineWidth(3.0);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-100.0, -34.0);
    glVertex2f(-50.0, -34.0);
    glVertex2f(-46.0, -38.0);
    glVertex2f(-46.0, -60.0);
    glVertex2f(-100.0, -60.0);
    glEnd();

    glColor3f(192.0f/255, 192.0f/255, 192.0f/255);
    glLineWidth(3.0);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-100.0, -36.0);
    glVertex2f(-54.0, -36.0);
    glVertex2f(-50.0, -40.0);
    glVertex2f(-50.0, -56.0);
    glVertex2f(-100.0, -56.0);
    glEnd();

    glColor3f(192.0f/255, 192.0f/255, 192.0f/255);
    glLineWidth(3.0);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-90.0, 66.0);
    glVertex2f(-90.0, -34.0);

    glEnd();

    glColor3f(192.0f/255, 192.0f/255, 192.0f/255);
    glLineWidth(3.0);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-84.0, 66.0);
    glVertex2f(-84.0, -34.0);
    glEnd();

    //design patterns right side
    glColor3f(192.0f/255, 192.0f/255, 192.0f/255);
    glLineWidth(3.0);
    glPushMatrix();
    glPointSize(5);
    glBegin(GL_LINE_STRIP);
    glVertex2f(46.0, 100.0);
    glVertex2f(46.0, 70.0);
    glVertex2f(48.0, 68.0);
    glVertex2f(100.0, 68.0);
    glEnd();
    glPopMatrix();

    glColor3f(192.0f/255, 192.0f/255, 192.0f/255);
    glLineWidth(3.0);
    glPushMatrix();
    glBegin(GL_LINE_STRIP);
    glVertex2f(44.0, 100.0);
    glVertex2f(44.0, 68.0);
    glVertex2f(46.0, 66.0);
    glVertex2f(100.0, 66.0);
    glEnd();
    glPopMatrix();

    glColor3f(192.0f/255, 192.0f/255, 192.0f/255);
    glLineWidth(3.0);
    glBegin(GL_LINE_STRIP);
    glVertex2f(100.0, -34.0);
    glVertex2f(50.0, -34.0);
    glVertex2f(46.0, -38.0);
    glVertex2f(46.0, -60.0);
    glVertex2f(100.0, -60.0);
    glEnd();

    glColor3f(192.0f/255, 192.0f/255, 192.0f/255);
    glLineWidth(3.0);
    glBegin(GL_LINE_STRIP);
    glVertex2f(100.0, -36.0);
    glVertex2f(54.0, -36.0);
    glVertex2f(50.0, -40.0);
    glVertex2f(50.0, -56.0);
    glVertex2f(100.0, -56.0);
    glEnd();

    glColor3f(192.0f/255, 192.0f/255, 192.0f/255);
    glLineWidth(3.0);
    glBegin(GL_LINE_STRIP);
    glVertex2f(90.0, 66.0);
    glVertex2f(90.0, -34.0);

    glEnd();

    glColor3f(192.0f/255, 192.0f/255, 192.0f/255);
    glLineWidth(3.0);
    glBegin(GL_LINE_STRIP);
    glVertex2f(84.0, 66.0);
    glVertex2f(84.0, -34.0);
    glEnd();
}

void light()
{
    //inner shadow

    glColor3f(36.0f/255, 65.0f/255, 83.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-90.5, 88.2);
    glVertex2f(-69.5, 88.2);
    glVertex2f(-69.5, 84.2);
    glVertex2f(-90.5, 84.2);

    glEnd();

    glColor3f(36.0f/255, 65.0f/255, 83.0f/255);
    drawFilledCircle(-90.5, 86.2f, 2.0f, 1000);
    drawFilledCircle(-69.5, 86.2f, 2.0f, 1000);

    // upper cyan body
    glColor3f(152.0f/255, 252.0f/255, 253.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-90.0, 89.0);
    glVertex2f(-70.0, 89.0);
    glVertex2f(-70.0, 85.0);
    glVertex2f(-90.0, 85.0);

    glEnd();

    glColor3f(152.0f/255, 252.0f/255, 253.0f/255); // Cyan color
    drawFilledCircle(-90.0, 87.0f, 2.0f, 1000);
    drawFilledCircle(-70.0, 87.0f, 2.0f, 1000);
    // upper white light
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_QUADS);
    glVertex2f(-89.0, 87.5);
    glVertex2f(-80.0, 87.5);
    glVertex2f(-80.0, 86.0);
    glVertex2f(-89.0, 86.0);

    glEnd();

    glColor3f(1.0,1.0,1.0); // white color
    drawFilledCircle(-89.0, 86.75f, 0.75f, 1000);
    drawFilledCircle(-80.0, 86.75f, 0.75f, 1000);


}

void window()
{
    glColor3f(128.0f/255, 239.0f/255, 256.0f/255);
    glBegin(GL_POLYGON);
    glVertex2f(-50.0, 44.0);
    glVertex2f(-40.0, 54.0);
    glVertex2f(40.0, 54.0);
    glVertex2f(50.0, 44.0);
    glVertex2f(70.0, 44.0);
    glVertex2f(70.0, -26.0);
    glVertex2f(50.0, -26.0);
    glVertex2f(40.0, -36.0);
    glVertex2f(-40.0, -36.0);
    glVertex2f(-50.0, -26.0);
    glVertex2f(-70.0, -26.0);
    glVertex2f(-70.0, 44.0);
    glEnd();

    glColor3f(128.0f/255, 239.0f/255, 256.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(70.0, 38.0);
    glVertex2f(76.0, 38.0);
    glVertex2f(76.0, -20.0);
    glVertex2f(70.0, -20.0);

    glEnd();
    glColor3f(128.0f/255, 239.0f/255, 256.0f/255);
    drawFilledCircle(70.0, 38.0, 6.0f, 1000);

    glColor3f(128.0f/255, 239.0f/255, 256.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-70.0, 38.0);
    glVertex2f(-76.0, 38.0);
    glVertex2f(-76.0, -20.0);
    glVertex2f(-70.0, -20.0);

    glEnd();
    glColor3f(128.0f/255, 239.0f/255, 256.0f/255);
    drawFilledCircle(-70.0, 38.0, 6.0f, 1000);

    //inner 01

    glPushMatrix();
    glScalef(0.95,0.97,1.0);
    glColor3f(1.0f/255, 114.0f/255, 108.0f/255);
    glBegin(GL_POLYGON);
    glVertex2f(-50.0, 44.0);
    glVertex2f(-40.0, 54.0);
    glVertex2f(40.0, 54.0);
    glVertex2f(50.0, 44.0);
    glVertex2f(70.0, 44.0);
    glVertex2f(70.0, -26.0);
    glVertex2f(50.0, -26.0);
    glVertex2f(40.0, -36.0);
    glVertex2f(-40.0, -36.0);
    glVertex2f(-50.0, -26.0);
    glVertex2f(-70.0, -26.0);
    glVertex2f(-70.0, 44.0);
    glEnd();

    glColor3f(1.0f/255, 114.0f/255, 108.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(70.0, 38.0);
    glVertex2f(76.0, 38.0);
    glVertex2f(76.0, -20.0);
    glVertex2f(70.0, -20.0);

    glEnd();
    glColor3f(1.0f/255, 114.0f/255, 108.0f/255);
    drawFilledCircle(70.0, 38.0, 6.0f, 1000);

    glColor3f(1.0f/255, 114.0f/255, 108.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-70.0, 38.0);
    glVertex2f(-76.0, 38.0);
    glVertex2f(-76.0, -20.0);
    glVertex2f(-70.0, -20.0);

    glEnd();
    glColor3f(1.0f/255, 114.0f/255, 108.0f/255);
    drawFilledCircle(-70.0, 38.0, 6.0f, 1000);
    glPopMatrix();


    //inner 2 black box

    glPushMatrix();
    glScalef(0.94,0.94,1.0);
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(-50.0, 44.0);
    glVertex2f(-40.0, 54.0);
    glVertex2f(40.0, 54.0);
    glVertex2f(50.0, 44.0);
    glVertex2f(70.0, 44.0);
    glVertex2f(70.0, -26.0);
    glVertex2f(50.0, -26.0);
    glVertex2f(40.0, -36.0);
    glVertex2f(-40.0, -36.0);
    glVertex2f(-50.0, -26.0);
    glVertex2f(-70.0, -26.0);
    glVertex2f(-70.0, 44.0);
    glEnd();

    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_QUADS);
    glVertex2f(70.0, 38.0);
    glVertex2f(76.0, 38.0);
    glVertex2f(76.0, -20.0);
    glVertex2f(70.0, -20.0);

    glEnd();
    glColor3f(0.0f,0.0f,0.0f);
    drawFilledCircle(70.0, 38.0, 6.0f, 1000);

    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-70.0, 38.0);
    glVertex2f(-76.0, 38.0);
    glVertex2f(-76.0, -20.0);
    glVertex2f(-70.0, -20.0);

    glEnd();
    glColor3f(0.0f,0.0f,0.0f);
    drawFilledCircle(-70.0, 38.0, 6.0f, 1000);
    glPopMatrix();

    //circle above window cyan and blue
    glColor3f(0.0f,1.0f,1.0f);
    drawFilledCircle(-74.0, 50.0, 1.5f, 1000);
    drawFilledCircle(-68.0, 50.0, 1.5f, 1000);
    drawFilledCircle(-62.0, 50.0, 1.5f, 1000);
    glColor3f(37.0f/255, 59.0f/255, 90.0f/255);
    drawFilledCircle(-56.0, 50.0, 1.5f, 1000);
    drawFilledCircle(-50.0, 50.0, 1.5f, 1000);

    //circle above window yellow and blue
    glColor3f(37.0f/255, 59.0f/255, 90.0f/255);
    drawFilledCircle(52.0, 50.0, 1.5f, 1000);
    drawFilledCircle(58.0, 50.0, 1.5f, 1000);
    drawFilledCircle(64.0, 50.0, 1.5f, 1000);
    glColor3f(1.0f,1.0f,0.0f);
    drawFilledCircle(70.0, 50.0, 1.5f, 1000);
    drawFilledCircle(76.0, 50.0, 1.5f, 1000);


    //starry background
    drawStarrySky(200, 0.0, 1.5, -38.0, 40.0, -34.0, 50.0);
    drawStarrySky(100, 0.0, 1.5, -70.0, -38.0, -24.0, 40.0);
    drawStarrySky(100, 0.0, 1.5, 38.0, 70.0, -24.0, 40.0);
    drawStarrySky(200, 0.0, 1.0, -38.0, 40.0, -30.0, 50.0);



    //start game box
    glColor3f(1.0f,0.0f,0.0f);

    //glClearColor(0.1294f, 0.1294f, 0.1294f,0.001);

    glBegin(GL_LINE_LOOP);
    glLineWidth(10);
    glVertex2f(-25.0, 15.0);
    glVertex2f(25.0, 15.0);
    glVertex2f(25.0, 0.0);
    glVertex2f(-25.0, 0.0);
    glEnd();
    // Four inner circles
    glColor3f(0.0f,1.0f,1.0f);
    drawFilledCircle(-22.0, 12.0, 1.0f, 1000);
    //glColor3f(1.0f,1.0f,0.0f);
    drawFilledCircle(22.0, 12.0, 1.0f, 1000);
    //glColor3f(1.0f,1.0f,0.0f);
    drawFilledCircle(22.0, 3.0, 1.0f, 1000);
    //glColor3f(1.0f,1.0f,0.0f);
    drawFilledCircle(-22.0, 3.0, 1.0f, 1000);

    glColor3f(1.0f,1.0f,0.0f);
    renderBitmapString(-13, 5.0, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, "START GAME");
    glLoadIdentity();

    glPushMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapString(-7, 40, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, "Welcome");
    renderBitmapString(-16, 30, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, "SPACE SENTINELS");
    glColor3f(0.0f, 1.0f, 0.0f);
    renderBitmapString(-8, -20, 0.0, GLUT_BITMAP_TIMES_ROMAN_10, "**Press Enter To Shoot**");
    renderBitmapString(-13, -25, 0.0, GLUT_BITMAP_TIMES_ROMAN_10, "**Use Arrow Keys To move spaceship**");
    renderBitmapString(-58, 20.5, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, "LEVEL 01");
    renderBitmapString(-58, 10.5, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, "LEVEL 02");
    renderBitmapString(-58, 0.5, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, "LEVEL 03");

    glColor3f(1.0f, 1.0f, 0.0f);
    renderBitmapString(41, 20.5, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, "DEFEAT BOSS");
    renderBitmapString(51, 10.5, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, "TO");
    renderBitmapString(43, 0.5, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, "WIN GAME");
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0f,0.0f,0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-65.0, 20.0);
    glVertex2f(-65.0, 25.0);
    glVertex2f(-60.0, 22.5);

    glEnd();
    glPopMatrix();




}



void monitor()
{
    //stand
    glColor3f(144.0f/255, 177.0f/255, 196.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-80.0, -83.0);
    glVertex2f(-60.0, -83.0);
    glVertex2f(-60.0, -43.0);
    glVertex2f(-80.0, -43.0);

    glEnd();

    //avove compartment of stand

    glColor3f(116.0f/255, 143.0f/255, 160.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-80.0, -43.0);
    glVertex2f(-60.0, -43.0);
    glVertex2f(-55.0, -38.0);
    glVertex2f(-85.0, -38.0);

    glEnd();

    //cyan box

    glColor3f(186.0f/255, 246.0f/255, 247.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-85.0, -38.0);
    glVertex2f(-55.0, -38.0);
    glVertex2f(-58.0, -18.0);
    glVertex2f(-82.0, -18.0);

    glEnd();

    //blue box

    glColor3f(0.0f/255, 0.0f/255, 56.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-83.0, -36.0);
    glVertex2f(-57.0, -36.0);
    glVertex2f(-60.0, -20.0);
    glVertex2f(-80.0, -20.0);

    glEnd();

    //progress bar cyan

    glColor3f(152.0f/255, 252.0f/255, 253.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-81.0, -34.0);
    glVertex2f(-59.0, -34.0);
    glVertex2f(-59.0, -33.0);
    glVertex2f(-81.0, -33.0);

    glEnd();

    glColor3f(152.0f/255, 252.0f/255, 253.0f/255); // Cyan color
    drawFilledCircle(-59.0f, -33.5f, 0.5f, 1000);
    drawFilledCircle(-81.0, -33.5f, 0.5f, 1000);

    //progress bar yellow

    glColor3f(255.0f/255, 255.0f/255, 0.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-81.0, -34.0);
    glVertex2f(-73.0, -34.0);
    glVertex2f(-73.0, -33.0);
    glVertex2f(-81.0, -33.0);

    glEnd();

    glColor3f(255.0f/255, 255.0f/255, 0.0f/255); // Cyan color
    drawFilledCircle(-73.0f, -33.5f, 0.5f, 1000);
    drawFilledCircle(-81.0, -33.5f, 0.5f, 1000);

    //cyan rectangular
    glColor3f(0.0f/255, 255.0f/255, 255.0f/255);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-81.0, -31.0);
    glVertex2f(-59.0, -31.0);
    glVertex2f(-61.0, -21.0);
    glVertex2f(-79.0, -21.0);

    glEnd();
    //drawBezierCurve();

    // cyan trend line
    glColor3f(255.0f/255, 0.0f/255, 0.0f/255);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-80.0, -29.0);
    glVertex2f(-73.0, -26.0);
    glVertex2f(-67.0, -27.0);
    glVertex2f(-63.0, -23.0);
    glEnd();

    //progress bar grey
    //glPushMatrix();
    // Scaling

    // Translation
    //glTranslatef(0.5, 15.0, 1.0); // Translate the object by 1 unit along the x-axis
    //glScalef(0.25, 0.25, 1.0); // Scale the object by 2 along x and y axes

    glColor3f(1.0,1.0,1.0);
    glBegin(GL_QUADS);
    glVertex2f(-81.0, -34.0+15.0);
    glVertex2f(-73.0, -34.0+15.0);
    glVertex2f(-73.0, -33.0+15.0);
    glVertex2f(-81.0, -33.0+15.0);

    glEnd();

    glColor3f(1.0,1.0,1.0);
    drawFilledCircle(-59.0f, -33.5f+15.0, 0.5f, 1000);
    drawFilledCircle(-81.0, -33.5f+15.0, 0.5f, 1000);
    //glPopMatrix();




    //BOX INSIDE THE STAND
    glColor3f(68.0f/255, 102.0f/255, 127.0f/255);
    glBegin(GL_QUADS);
    glVertex2f(-77.0, -52.0);
    glVertex2f(-63.0, -52.0);
    glVertex2f(-63.0, -45.0);
    glVertex2f(-77.0, -45.0);

    glEnd();

    glColor3f(0.0f/255, 255.0f/255, 255.0f/255);
    drawFilledCircle(-75.0f, -47.0f, 1.0f, 1000);
    drawFilledCircle(-72.0f, -47.0f, 1.0f, 1000);
    drawFilledCircle(-69.0f, -47.0f, 1.0f, 1000);
    drawFilledCircle(-66.0f, -47.0f, 1.0f, 1000);
    drawFilledCircle(-75.0f, -49.5f, 1.0f, 1000);
    drawFilledCircle(-72.0f, -49.5f, 1.0f, 1000);
    drawFilledCircle(-69.0f, -49.5f, 1.0f, 1000);
    drawFilledCircle(-66.0f, -49.5f, 1.0f, 1000);



}




void scoreBoardBox()
{
    glPushMatrix();
    glTranslatef(-95.0,90.0,1);
    glColor3f(1.0,1.0,0.0);
    glBegin(GL_QUADS);
    glVertex2f(-20.0, 10.0);
    glVertex2f(-20.0, -10.0);
    glVertex2f(20.0, -10.0);
    glVertex2f(20.0, 10.0);
    glPopMatrix();
    glEnd();
}


void display() {
    glClearColor(0.85f, 0.85f, 0.9f, 1.0f);
    glClearColor(0.1294f, 0.1294f, 0.1294f, 1.0f);
 // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer (background)
    glLoadIdentity();



    /*
    for (int i = 0; i < numStarsDrawn; ++i) {
        drawStars1(stars[i].x, stars[i].y, stars[i].size, stars[i].r, stars[i].g, stars[i].b);
    }

    //drawStars(1000); // Draw 100 stars
    //drawStar();
    drawStarrySky(1000,0.5,2.5,-100,100,-100,100);

    */
    drawStars();
    spaceShip();
    drawBullets();
    /*

    if (bossActive) {
        drawBoss(); // Draw the boss
    } else {
        // Draw the game over screen
        cout<<"Game over"<<endl;
    }
    */


    // Draw enemies
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for enemies
    for (const auto& enemy : enemies) {
        if (enemy.active) {
            glPushMatrix(); // Save current matrix
            glTranslatef(enemy.x, enemy.y, 0.0f);

            drawEnemy();
            // Draw enemy box
            //glBegin(GL_QUADS);
            //glVertex2f(-10.0f, -10.0f); // Bottom-left
            //glVertex2f(10.0f, -10.0f);  // Bottom-right
            //glVertex2f(10.0f, 10.0f);   // Top-right
            //glVertex2f(-10.0f, 10.0f);  // Top-left
            //glEnd();

            glPopMatrix(); // Restore previous matrix
        }
    }

    // Draw the score board
    drawScoreBoard();

    // Draw the health bar
    drawHealthBar();

    // Swap buffers to display the rendered frame
    //glutSwapBuffers();


    // Draw boss
    if (boss.active) {
        glColor3f(0.0f, 1.0f, 0.0f); // Green color for boss
        glPushMatrix(); // Save current matrix
        glTranslatef(boss.x, boss.y-5, 0.0f);

        drawBoss();
        // Draw boss box
        //glBegin(GL_QUADS);
        //glVertex2f(-20.0f, -10.0f); // Bottom-left
        //glVertex2f(20.0f, -10.0f);  // Bottom-right
        //glVertex2f(20.0f, 10.0f);   // Top-right
        //glVertex2f(-20.0f, 10.0f);  // Top-left
        //glEnd();

        glPopMatrix(); // Restore previous matrix
    }



    glFlush(); // Render now
    glutSwapBuffers();

}


void moveSpaceship()
{
    if(xPosition < 900 ){
        xPosition += 0.9f;
    }
}








class RainScreen {
public:
    void setup() {
        srand(time(NULL));
        glClearColor(0.0 / 255, 0.0 / 255, 0.0 / 255, 1.0);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-100, 100, -100, 100, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        for (int i = 0; i < 500; i++) {
            drops.push_back(Drop());
        }
    }

    void draw() {
        glClear(GL_COLOR_BUFFER_BIT);



        // Draw raindrops
        for (int i = 0; i < drops.size(); i++) {
            drops[i].fall();
            drops[i].show();
        }




        // Draw rectangle
        glPushMatrix();
        glTranslatef(0.0,20.0,1);
        glScalef(2.0,2.0,1);
        glColor3f(0.0f, 206.0/255.0, 209.0f/255.0); // Gray color
        glBegin(GL_POLYGON);
        glVertex2f(-18, 15);
        glVertex2f(18, 15);
        glVertex2f(18, 13);
        glVertex2f(20, 13);
        glVertex2f(20, -13);
        glVertex2f(18, -13);
        glVertex2f(18, -15);
        glVertex2f(-18, -15);
        glVertex2f(-18, -13);
        glVertex2f(-20, -13);
        glVertex2f(-20, 13);
        glVertex2f(-18, 13);
        glEnd();
        drawFilledCircle(-18.0,13.0,2.0,1000);
        drawFilledCircle(18.0,13.0,2.0,1000);
        drawFilledCircle(18.0,-13.0,2.0,1000);
        drawFilledCircle(-18.0,-13.0,2.0,1000);

        glPopMatrix();
        spaceShip();
        glColor3f(250.0/255.0,38.0/255.0,160.0/255.0);
        renderBitmapString(-20, 30, 0.0, GLUT_BITMAP_HELVETICA_18, "HYPERSPACE ACTIVATED");
        glColor3f(178.0/255.0,34.0/255.0,34.0/255.0);
        renderBitmapString(-25, 20, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, "Enemy Escape Was Successful");
        glColor3f(128.0/255.0,0.0/255.0,0.0/255.0);
        renderBitmapString(-27.0, 10, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, "**Press 'x' To End The Game**");

        // Write blinking text
        if (blinkOn) {
            glColor3f(1.0f, 1.0f, 0.0f); // White color
            glRasterPos2f(-10, -70);
            std::string text = "GAME OVER !";
            for (char c : text) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
            }
        }

        glutSwapBuffers();

        // Toggle blinkOn every interval
        if (blinkCounter++ >= blinkInterval) {
            blinkOn = !blinkOn;
            blinkCounter = 0;
        }
    }

private:
    class Drop {
    public:
        float x, y, z;
        float len;
        float yspeed;
        float r, g, b; // Color components

        Drop() {
            x = randomFloat(-100, 100);
            y = randomFloat(-100, 100); // Start above the visible screen area
            z = randomFloat(0, 20);
            len = mapValue(z, 0, 20, 10, 20);
            yspeed = mapValue(z, 0, 20, 1, 20); // Ensure yspeed is positive

            // Randomize color components
            r = randomFloat(0, 1);
            g = randomFloat(0, 1);
            b = randomFloat(0, 1);
        }

        void fall() {
            y -= yspeed; // Update y position to make raindrops fall downwards
            float grav = mapValue(z, 0, 20, 0, 0.2);
            yspeed += grav;

            if (y < -100) { // If the raindrop goes below the bottom of the screen
                y = randomFloat(100, 200); // Reset it above the visible screen area
                x = randomFloat(-100, 100); // Randomize x position
                z = randomFloat(0, 20); // Randomize z position
                len = mapValue(z, 0, 20, 10, 20); // Update len based on z position
                yspeed = mapValue(z, 0, 20, 4, 10); // Update yspeed based on z position

                // Randomize color components for a new color
                r = randomFloat(0, 1);
                g = randomFloat(0, 1);
                b = randomFloat(0, 1);
            }
        }

        void show() {
            float thick = mapValue(z, 0, 20, 1, 3);
            glColor3f(r, g, b); // Set color
            glLineWidth(thick);
            glBegin(GL_LINES);
            glVertex2f(x, y);
            glVertex2f(x, y + len);
            glEnd();
        }

    private:
        float randomFloat(float min, float max) {
            return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
        }

        float mapValue(float value, float start1, float stop1, float start2, float stop2) {
            return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
        }
    };

    std::vector<Drop> drops;
    bool blinkOn = true;
    int blinkCounter = 0;
    const int blinkInterval = 30; // Adjust blink speed here
};

RainScreen rainScreen;

void updateRain(int value) {
    glutPostRedisplay(); // Call redraw function
    glutTimerFunc(16, updateRain, 0); // Update every 16 milliseconds
}

void switchToRainScreen() {
    glutDisplayFunc([]() { rainScreen.draw(); });
    rainScreen.setup();
    glutTimerFunc(16, updateRain, 0); // Call update function every 16 milliseconds
}



void fscene()
{


    glPushMatrix();
//    glTranslatef(xPosition,yPositio8n,0.0);


    glLineWidth(2.0);
    // Begin drawing lines
   glColor3f(0.337f, 0.725f, 1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-15.0, 0.0);
        glVertex2f(-17.0, 0.0);
        glVertex2f(-17.0, 0.0);
        glVertex2f(-27.0, 14.0);
        glVertex2f(-27.0, 14.0);
        glVertex2f(-40.0, 18.0);
        glVertex2f(-40.0, 18.0);
        glVertex2f(-40.0, 20.0);
        glVertex2f(-40.0, 20.0);
        glVertex2f(-27.0, 16.0);
         glVertex2f(-27.0, 16.0);
        glVertex2f(-17.0, 32.0);
        glVertex2f(-17.0, 32.0);
         glVertex2f(-15.0, 31.0);
         glVertex2f(-15.0, 31.0);
         glVertex2f(-25.0, 15.0);
         glVertex2f(-25.0, 15.0);
         glVertex2f(-15.0, 0.0);
    glEnd();


       glLineWidth(2.0);

    // Begin drawing lines

   glColor3f(0.337f, 0.725f, 1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(15.0, 0.0);
        glVertex2f(17.0, 0.0);
        glVertex2f(17.0, 0.0);
        glVertex2f(27.0, 14.0);
        glVertex2f(27.0, 14.0);
        glVertex2f(40.0, 18.0);
        glVertex2f(40.0, 18.0);
        glVertex2f(40.0, 20.0);
        glVertex2f(40.0, 20.0);
        glVertex2f(27.0, 16.0);
         glVertex2f(27.0, 16.0);
        glVertex2f(17.0, 32.0);
        glVertex2f(17.0, 32.0);
         glVertex2f(15.0, 31.0);
         glVertex2f(15.0, 31.0);
         glVertex2f(25.0, 15.0);
         glVertex2f(25.0, 15.0);
         glVertex2f(15.0, 0.0);
    glEnd();
      glLineWidth(2.0);

    // Begin drawing lines
//middle
   glColor3f(0.337f, 0.725f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(-2.0, 0.0);
    glVertex2f(2.0, 0.0);

     glEnd();



           // Set line width
    glLineWidth(2.0);

    // Set color to white
   glColor3f(0.337f, 0.725f, 1.0f);// White color

    // Begin drawing a line loop
    glBegin(GL_LINE_LOOP);
        glVertex2f(-2.0, 0.0);
        glVertex2f(-2.0, 20.0);
        glVertex2f(-27.0, 40.0);
        glVertex2f(-20.0, 40.0);
        glVertex2f(0.0, 25.0);
        glVertex2f(20.0, 40.0);
        glVertex2f(27.0, 40.0);
        glVertex2f(2.0, 20.0); // This vertex repeated to close the shape nicely in GL_LINE_LOOP
        glVertex2f(2.0, 0.0); // This vertex repeated to close the shape nicely in GL_LINE_LOOP
    glEnd();
   /* glLineWidth(1.0);
   glColor3f(1.0f, 1.0f, 1.0f); // Red color
    glBegin(GL_LINES);
    glVertex2f(-40.0, 20.0);
    glVertex2f(-26.0, 16.0);
    glVertex2f(-26.0, 16.0);
    glVertex2f(-17.0, 33.0);

     glEnd(); */

     glLineWidth(2.0);
     glColor3f(0.337f, 0.725f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-17.0, 0.0);
    glVertex2f(17.0, 0.0);
    glVertex2f(17.0, -2.0);
    glVertex2f(-17.0, -2.0);

     glEnd();

     glLineWidth(2.0);
   glColor3f(0.337f, 0.725f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-17.0, 0.0);
       glVertex2f(-17.0, -2.0);
    glVertex2f(-40.0, -22.0);

    glVertex2f(-40.0, -18.0);



     glEnd();
      glLineWidth(2.0);
   glColor3f(0.337f, 0.725f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(17.0, 0.0);
       glVertex2f(17.0, -2.0);
    glVertex2f(40.0, -22.0);

    glVertex2f(40.0, -18.0);



     glEnd();


       glLineWidth(2.0);
    glColor3f(0.337f, 0.725f, 1.0f);
    glBegin(GL_LINE_LOOP);

    glVertex2f(-17.0, -2.0);
    glVertex2f(-12.0, -15.0);
    glVertex2f(-8.0, -15.0);
    glVertex2f(-5.0, -25.0);
    glVertex2f(5.0, -25.0);
    glVertex2f(8.0, -15.0);
    glVertex2f(12.0, -15.0);
    glVertex2f(17.0, -2.0);



     glEnd();

    glLineWidth(2.0);
   glColor3f(0.337f, 0.725f, 1.0f);
    glBegin(GL_LINE_LOOP);

    glVertex2f(-16.0, -2.0);
    glVertex2f(-11.0, -13.0);
    glVertex2f(-7.0, -13.0);
    glVertex2f(-4.0, -23.0);
    glVertex2f(4.0, -23.0);
    glVertex2f(7.0, -13.0);
    glVertex2f(11.0, -13.0);
    glVertex2f(16.0, -2.0);



     glEnd();

      glLineWidth(2.0);
   glColor3f(0.337f, 0.725f, 1.0f);
    glBegin(GL_LINE_LOOP);

    glVertex2f(-14.0, -11.0);
    glVertex2f(-14.0, -10.0);
    glVertex2f(-40.0, -35.0);
    glVertex2f(-40.0, -38.0);

     glEnd();

       glLineWidth(2.0);
   glColor3f(0.337f, 0.725f, 1.0f);
    glBegin(GL_LINE_LOOP);


    glVertex2f(14.0, -11.0);
    glVertex2f(14.0, -10.0);
    glVertex2f(40.0, -35.0);
    glVertex2f(40.0, -38.0);




     glEnd();






}


void scene()
{



    //drawStarrySky(200);
    glPushMatrix();
//    glTranslatef(xPosition,yPosition,0.0);



//1T
/*
    glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_POLYGON);
    glVertex2f(-2.0, 0.0);
    glVertex2f(2.0, 0.0);
    glVertex2f(2.0, 25);
    glVertex2f(-2.0, 25);


    glEnd();
    //2t
     glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_POLYGON);
    glVertex2f(0.0, 18.0);
    glVertex2f(25.0, 40.0);
    glVertex2f(20.0, 40.0);
    glVertex2f(0.0, 25.0);


    glEnd();
     glColor3f(0.0f, 0.0f, 0.0f);
     glBegin(GL_POLYGON);
    glVertex2f(0.0, 18.0);
    glVertex2f(-25.0, 40.0);
    glVertex2f(-20.0, 40.0);
    glVertex2f(0.0, 25.0);


    glEnd();

     */
     glColor3f(1.0, 0.0f, 0.0f);
     renderBitmapString(-5.0, -10, 0.0f, GLUT_BITMAP_9_BY_15, "WINNER");



 glFlush();


   // glEnd();








}

void winnerScreen() {

    glClearColor(0.1294f, 0.1294f, 0.1294f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    /*
    for (size_t i = 0; i < stars.size(); i++) {
        stars[i].update(speed);
        stars[i].show();
    }
      glLoadIdentity();
      glScalef(2.5f, 3.0f, 1.0f);

      */
      drawStarrySky(2000, 0.0, 1.5, -100.0, 100.0, -100.0, 100.0);
     fscene();
     scene();
     glFlush();

    glutSwapBuffers();

}



void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            if(yPosition<110)
              yPosition += 0.9f; // Move up
            break;
        case GLUT_KEY_DOWN:
            if(yPosition>-50)
              yPosition -= 0.9f; // Move down
            break;
        case GLUT_KEY_LEFT:
            if(xPosition>-90)
              xPosition -= 0.9f; // Move left
            //moveSpaceship();
            break;
        case GLUT_KEY_RIGHT:
            //xPosition += 0.9f; // Move right
            if(xPosition<100){
                xPosition += 0.9f;
            }
            //moveSpaceship();
            break;
    }

    glutPostRedisplay(); // Redraw the scene
}



void keyboard(unsigned char key, int x, int y) {
    switch (key) {


        case 27:

            glutDisplayFunc(cockpit);

            glutPostRedisplay();
            break;


        case 13: // ASCII code for Enter key
            // Start shooting when Enter key is pressed
            shooting = true;
            // Calculate the position of the tip of the spaceship

            GLfloat tipX = xPosition;
            GLfloat tipY = yPosition + 5.0f; // Assuming the spaceship height is 32.0f
            bullets.push_back(std::make_pair(tipX, tipY)); // Create bullet at the tip position
            soundBullet();
            break;

        //case 'n':
            //exit(0);
            //break;
    }
}



void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Convert mouse coordinates to OpenGL coordinates
        float mouseX = ((float)x / 640.0) * 200.0 - 100.0;
        float mouseY = 100.0 - ((float)y / 480.0) * 200.0;


        // Check if the click is inside the square
        if (mouseX >= -50.0 && mouseX <= 50.0 && mouseY >= 0.0 && mouseY <= 30.0) {
            std::cout << "Clicked inside the square!" << std::endl;
            gameScreen = true ;
            glutDisplayFunc(display);
            //switchToRainScreen();
            //glutDisplayFunc(backGround);

            glutPostRedisplay();
            // Perform actions here
        }
    }
}



void update(int value) {
    if (shooting) {
        // Move bullets upward
        for (auto& bullet : bullets) {
            bullet.second += 0.9f; // Adjust the value as needed
        }
    }

    // Check if the player reaches 100 points and the boss is not active
    if (score >= 100 && !bossActive) {
        spawnBoss(); // Function to initialize boss
        bossActive = true;
    }

    /*
    // Move stars down by decrementing starYPos
    starYPos -= 1.0f;


    // Draw next group of stars if needed
    if (numStarsDrawn < numStarsPerGroup * numGroups) {
        generateStars();
    } else {
        // Reset for next group of stars
        starYPos = 100.0f;
        numStarsDrawn = 0;
        currentGroup = (currentGroup + 1) % numGroups;
    }
    */

    // Spawn enemies periodically
    if (rand() % 100 == 0) {
        spawnEnemy();
    }

    // Update enemies
    updateEnemies();


    // Check if the score reaches 100 to spawn the boss
    if (score >= 5
         && !boss.active) {
        spawnBoss();
    }

    // Update boss position and handle collisions
    updateBoss();


    glutPostRedisplay(); // Redraw the scene
    glutTimerFunc(16, update, 0); // Call update function again after 16 milliseconds
}



void reshape(int w,int h)
{
    glViewport(0,0,(GLsizei)w,(GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluOrtho2D(-50,50,-50,50);
    gluOrtho2D(-100,100,-100,100);
    glMatrixMode(GL_MODELVIEW);
}


void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set background color to white

}
void init1() {
    srand(static_cast<unsigned int>(time(NULL)));
    for (int i = 0; i < 1000; i++) {
        stars.push_back(Star());
    }

    speed = 1;
}




int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(640, 480); // Set the window's initial width & height
    glutInitWindowPosition(80, 50);  // Set the window's initial position according to the monitor
    glutCreateWindow("Spaceship Game"); // Create a window with the given title
    soundGame();
    // Initialize boss object inside main function
    //boss.x = 0;
    //boss.y = 90;
    //boss.width = 20;
    //boss.height = 20;
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glutDisplayFunc(information); // Register display callback handler for window re-paint
    //glutDisplayFunc(winnerScreen); // Register display callback handler for window re-paint
    //switchToRainScreen();

    //switchToRainScreen();

    //glutReshapeFunc();
    //glutReshapeFunc(reshape);
    /*
    if(introPage){
        glutTimerFunc(0, updateRain, 0); // Start update loop
    }
    */
    //glutTimerFunc(0, updateRain, 0);



    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys); // Register special key event handler
    glutKeyboardFunc(keyboard); // Register keyboard event handler
    glutMouseFunc(mouse);
    init();
    init1();
    glutTimerFunc(0, update, 0);

    glutMainLoop(); // Enter the event-processing loop

    return 0;
}





