#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <time.h>

#define ROAD_LEFT 170
#define ROAD_RIGHT 470
#define CAR_W 40
#define CAR_H 60
#define SCREEN_H 350
#define MAX_FUEL 100.0
#define FUEL_CONSUMPTION 0.2
#define FUEL_REFILL 20.0

int carX, carY;
int enemyX[3], enemyY[3];
int enemyColor[3];
int foodX, foodY; // Fuel/Gas can
int foodActive = 0;
int score;
int highScore = 0;
int gameOver;
int page = 0; // Double buffering page
int enemySpeed;
int gameDelay;
float fuel;
int sceneryX[6], sceneryY[6], sceneryType[6]; // 0 for tree, 1 for rock
int musicTimer = 0;
int musicIndex = 0;
int melody[] = {262, 294, 330, 349, 392, 440, 494, 523, 494, 440, 392, 349, 330, 294};
int melodyLen = 14;
int musicOn = 1;
int paused = 0;

void loadHighScore() {
    FILE *fp = fopen("HIGHSCORE.TXT", "r");
    if (fp != NULL) {
        fscanf(fp, "%d", &highScore);
        fclose(fp);
    } else {
        highScore = 0;
    }
}

void saveHighScore() {
    FILE *fp = fopen("HIGHSCORE.TXT", "w");
    if (fp != NULL) {
        fprintf(fp, "%d", highScore);
        fclose(fp);
    }
}

void drawCar(int x, int y, int color) {
    // Wheels
    setfillstyle(SOLID_FILL, BLACK);
    bar(x - 5, y + 10, x, y + 20); // Front Left
    bar(x + CAR_W, y + 10, x + CAR_W + 5, y + 20); // Front Right
    bar(x - 5, y + 40, x, y + 50); // Rear Left
    bar(x + CAR_W, y + 40, x + CAR_W + 5, y + 50); // Rear Right

    // Body
    setcolor(color);
    setfillstyle(SOLID_FILL, color);
    bar(x, y, x + CAR_W, y + CAR_H);

    // Windshield (Cyan/Light Blue)
    setfillstyle(SOLID_FILL, LIGHTCYAN);
    bar(x + 5, y + 10, x + CAR_W - 5, y + 25);
    
    // Racing Stripe or Roof detail
    setfillstyle(SOLID_FILL, color == RED ? YELLOW : BLACK);
    bar(x + 15, y + 30, x + 25, y + 55);
}

void drawFuelCan(int x, int y) {
    setfillstyle(SOLID_FILL, LIGHTRED); // Red Can
    bar(x, y, x + 20, y + 30);
    setcolor(YELLOW);
    rectangle(x, y, x + 20, y + 30);
    outtextxy(x + 4, y + 8, "F");
}

void drawScenery(int x, int y, int type) {
    if (type == 0) { // Tree
        setfillstyle(SOLID_FILL, BROWN);
        bar(x + 15, y + 30, x + 25, y + 50); // Trunk
        setcolor(GREEN);
        setfillstyle(SOLID_FILL, GREEN);
        fillellipse(x + 20, y + 20, 20, 20); // Leaves
        setfillstyle(SOLID_FILL, DARKGRAY); // Adding some detail to tree
        fillellipse(x + 15, y + 15, 2, 2); 
    } else { // Rock
        setfillstyle(SOLID_FILL, LIGHTGRAY);
        sector(x + 20, y + 40, 0, 180, 20, 15); // Rock shape
        setcolor(DARKGRAY);
        line(x + 10, y + 35, x + 30, y + 35); // Rock detail
    }
}

void playMusic() {
    if (!musicOn) return;
    
    musicTimer++;
    if (musicTimer % 8 == 0) { // Play a note every 8 frames
        sound(melody[musicIndex]);
        musicIndex = (musicIndex + 1) % melodyLen;
    } else if (musicTimer % 8 == 2) { // Short duration for each note
        nosound();
    }
}

void setup() {
    // Use VGA driver and VGAMED mode (640x350) which supports 2 pages for double buffering
    int gd = VGA, gm = 1; 
    int i;
    
    // Initialize Graphics
    initgraph(&gd, &gm, "C:\\TURBOC3\\BGI");
    
    if (graphresult() != grOk) {
        printf("Graphics Error: %s\n", grapherrormsg(graphresult()));
        printf("Press any key to exit...");
        getch();
        exit(1);
    }
    
    randomize();
    loadHighScore();
    
    carX = 300;
    carY = SCREEN_H - CAR_H - 20; // Adjust for 350 height
    score = 0;
    gameOver = 0;
    
    // Intro Music
    sound(400); delay(100);
    sound(500); delay(100);
    sound(600); delay(200);
    nosound();
    
    // Initialize enemies
    for(i=0; i<3; i++) {
        enemyX[i] = ROAD_LEFT + random(ROAD_RIGHT - ROAD_LEFT - CAR_W);
        enemyY[i] = -100 - (i * 200); // Spaced further apart
        enemyColor[i] = 1 + random(14); 
        if (enemyColor[i] == RED) enemyColor[i] = BLUE; 
    }

    // Initialize Scenery
    for(i=0; i<6; i++) {
        if (i < 3) // Left side
            sceneryX[i] = random(ROAD_LEFT - 40);
        else // Right side
            sceneryX[i] = ROAD_RIGHT + 20 + random(640 - ROAD_RIGHT - 60);
        
        sceneryY[i] = random(SCREEN_H) - SCREEN_H;
        sceneryType[i] = random(2);
    }
    
    foodActive = 0;
    foodY = -500;
}

void resetGame() {
    int i;
    
    // Reset player
    carX = 300;
    carY = SCREEN_H - CAR_H - 20;
    score = 0;
    gameOver = 0;
    page = 0;
    musicTimer = 0;
    musicIndex = 0;
    fuel = MAX_FUEL;
    
    // Reset enemies
    for(i=0; i<3; i++) {
        enemyX[i] = ROAD_LEFT + random(ROAD_RIGHT - ROAD_LEFT - CAR_W);
        enemyY[i] = -100 - (i * 200); // Spaced further apart
        enemyColor[i] = 1 + random(14); 
        if (enemyColor[i] == RED) enemyColor[i] = BLUE; 
    }
    
    // Reset Scenery
    for(i=0; i<6; i++) {
        if (i < 3) // Left side
            sceneryX[i] = random(ROAD_LEFT - 40);
        else // Right side
            sceneryX[i] = ROAD_RIGHT + 20 + random(640 - ROAD_RIGHT - 60);
        
        sceneryY[i] = random(SCREEN_H) - SCREEN_H;
        sceneryType[i] = random(2);
    }
    
    foodActive = 0;
    foodY = -500;
}

void draw() {
    int i;
    char strBuf[30];
    
    // Set active page to the one we are NOT viewing (back buffer)
    setactivepage(page);
    cleardevice();
    
    // Draw Grass
    setfillstyle(SOLID_FILL, GREEN);
    bar(0, 0, ROAD_LEFT, SCREEN_H); 
    bar(ROAD_RIGHT, 0, 640, SCREEN_H); 

    // Draw Scenery
    for(i=0; i<6; i++) {
        drawScenery(sceneryX[i], sceneryY[i], sceneryType[i]);
    }
    
    // Draw Road
    setfillstyle(SOLID_FILL, DARKGRAY); 
    bar(ROAD_LEFT, 0, ROAD_RIGHT, SCREEN_H);
    
    // Draw Road Borders
    setcolor(WHITE);
    line(ROAD_LEFT, 0, ROAD_LEFT, SCREEN_H);
    line(ROAD_RIGHT, 0, ROAD_RIGHT, SCREEN_H);
    
    // Draw Center Lines
    setlinestyle(DASHED_LINE, 1, 3);
    line(320, 0, 320, SCREEN_H);
    setlinestyle(SOLID_LINE, 1, 1);
    
    // Draw Fuel Item
    if (foodActive) {
        drawFuelCan(foodX, foodY);
    }
    
    // Draw Player Car
    drawCar(carX, carY, RED);

    // Draw Enemies
    for(i=0; i<3; i++) {
        if(enemyY[i] > -CAR_H && enemyY[i] < SCREEN_H) {
             drawCar(enemyX[i], enemyY[i], enemyColor[i]);
        }
    }
    
    // UI
    setcolor(WHITE);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    
    // Score
    sprintf(strBuf, "Score: %d", score);
    outtextxy(10, 10, strBuf);
    
    // High Score
    sprintf(strBuf, "Hi-Score: %d", highScore);
    outtextxy(10, 25, strBuf);
    
    // Instructions
    outtextxy(10, 310, "P: Pause");
    outtextxy(10, 330, "X: Exit");
    
    // Fuel Bar
    outtextxy(500, 10, "FUEL");
    setfillstyle(SOLID_FILL, WHITE);
    rectangle(500, 25, 600, 40); // Fuel container
    if (fuel > 50) setfillstyle(SOLID_FILL, LIGHTGREEN);
    else if (fuel > 20) setfillstyle(SOLID_FILL, YELLOW);
    else setfillstyle(SOLID_FILL, RED);
    
    if (fuel > 0) {
        bar(501, 26, 501 + (int)fuel, 39);
    }
    
    if (paused) {
        setcolor(YELLOW);
        settextstyle(TRIPLEX_FONT, HORIZ_DIR, 4);
        outtextxy(230, 150, "PAUSED");
    }
    
    // Switch visual page to the one we just drew
    setvisualpage(page);
    
    // Flip page index for next frame
    page = 1 - page;
}

void input() {
    if (kbhit()) {
        char ch = getch();
        if (ch == 0) ch = getch(); 
        
        if (ch == 'p' || ch == 'P') {
            paused = !paused;
            // Draw "Paused" immediately so user sees it
            if (paused) {
                // Since we are double buffering, we draw on the visible page for immediate feedback
                // or just let the next draw() loop handle it which is fine.
            }
        }
        
        if (!paused) {
            // Move Left
            if ((ch == 'a' || ch == 'A' || ch == 75) && carX - 20 >= ROAD_LEFT) {
                carX -= 20;
            }
            // Move Right
            if ((ch == 'd' || ch == 'D' || ch == 77) && carX + 20 + CAR_W <= ROAD_RIGHT) {
                carX += 20;
            }
        }
        
        // Exit
        if (ch == 'x' || ch == 'X') {
            gameOver = 1;
        }
    }
}

// Adjusted to include wheels (extra 5 pixels on left and right)
// Visual width is effectively CAR_W + 10
int carsOverlap(int x1, int y1, int x2, int y2) {
    int w1_left = x1 - 5;
    int w1_right = x1 + CAR_W + 5;
    int w2_left = x2 - 5;
    int w2_right = x2 + CAR_W + 5;

    return (w1_left < w2_right) && (w1_right > w2_left) &&
           (y1 < y2 + CAR_H) && (y1 + CAR_H > y2);
}

int checkFuelCollision(int cX, int cY, int fX, int fY) {
    // Car includes wheels
    int car_left = cX - 5;
    int car_right = cX + CAR_W + 5;
    
    // Fuel can is 20 wide
    return (car_left < fX + 20) && (car_right > fX) &&
           (cY < fY + 30) && (cY + CAR_H > fY);
}

void logic() {
    int i, j, attempts;
    int newX, newY;
    int overlap;
    
    if (paused) return;
    
    // Fuel Consumption
    fuel -= FUEL_CONSUMPTION;
    if (fuel <= 0) {
        fuel = 0;
        gameOver = 1; // Out of gas!
        return;
    }
    
    // Handle Fuel Item
    if (foodActive) {
        foodY += enemySpeed;
        if (foodY > SCREEN_H) {
            foodActive = 0; // Missed it
        }
        if (checkFuelCollision(carX, carY, foodX, foodY)) {
            fuel += FUEL_REFILL;
            if (fuel > MAX_FUEL) fuel = MAX_FUEL;
            foodActive = 0;
            score += 50; // Bonus for fuel
            sound(1000); delay(50); nosound();
        }
    } else {
        // Randomly spawn fuel
        if (random(100) < 2) { // 2% chance per frame
             attempts = 0;
             do {
                 overlap = 0;
                 foodX = ROAD_LEFT + random(ROAD_RIGHT - ROAD_LEFT - 20);
                 foodY = -50;
                 
                 // Check overlap with active enemies
                 for(j=0; j<3; j++) {
                     if (carsOverlap(foodX, foodY, enemyX[j], enemyY[j])) {
                         overlap = 1;
                         break;
                     }
                 }
                 attempts++;
             } while(overlap && attempts < 20);
             
             if (!overlap) {
                 foodActive = 1;
             }
        }
    }
    
    for(i=0; i<3; i++) {
        enemyY[i] += enemySpeed;
        
        // Reset enemy
        if (enemyY[i] > SCREEN_H) {
            // Respawn with better spacing logic
            // Push it further up to avoid immediate bunching
            enemyY[i] = -80 - random(100);
            
            attempts = 0;
            do {
                overlap = 0;
                newX = ROAD_LEFT + random(ROAD_RIGHT - ROAD_LEFT - CAR_W);
                newY = enemyY[i]; 
                
                // Check overlap with other enemies
                for(j=0; j<3; j++) {
                    if (i != j) {
                        // Check if the new position overlaps with existing car
                        // Add a safety buffer of 10 pixels
                        if (carsOverlap(newX, newY, enemyX[j], enemyY[j])) {
                            overlap = 1;
                            break;
                        }
                        // Also check simple vertical proximity to prevent "stacked" cars
                        // If they are too close vertically, force a different X or retry
                        if (abs(newY - enemyY[j]) < CAR_H + 20) {
                             if (abs(newX - enemyX[j]) < CAR_W + 10) {
                                 overlap = 1;
                                 break;
                             }
                        }
                    }
                }
                
                // Check overlap with Fuel
                if (foodActive) {
                    if (checkFuelCollision(newX, newY, foodX, foodY)) {
                         overlap = 1;
                    }
                }
                
                attempts++;
            } while(overlap && attempts < 50);
            
            // If still overlapping after 50 tries, push this car WAY back
             if (overlap) {
                newX = ROAD_LEFT + random(ROAD_RIGHT - ROAD_LEFT - CAR_W);
                enemyY[i] -= 200; // Delay its arrival significantly
            }

            enemyX[i] = newX;
            enemyColor[i] = 1 + random(14);
            if (enemyColor[i] == RED) enemyColor[i] = BLUE; 
            
            score += 10;
            
            // Score Sound
            sound(800); delay(10); nosound();
        }
        
        // Collision with player
        if (carsOverlap(carX, carY, enemyX[i], enemyY[i])) {
            gameOver = 1;
        }
    }

    // Move Scenery
    for(i=0; i<6; i++) {
        sceneryY[i] += enemySpeed;
        if (sceneryY[i] > SCREEN_H) {
            sceneryY[i] = -100;
            if (i < 3) 
                sceneryX[i] = random(ROAD_LEFT - 40);
            else 
                sceneryX[i] = ROAD_RIGHT + 20 + random(640 - ROAD_RIGHT - 60);
            sceneryType[i] = random(2);
        }
    }
}

int game_over_screen() {
    char msg[50];
    char ch;
    int i;
    int isHighScore = 0;
    
    // Check High Score
    if (score > highScore) {
        highScore = score;
        saveHighScore();
        isHighScore = 1;
    }
    
    setvisualpage(0);
    setactivepage(0);
    cleardevice();
    
    // Crash Sound Effect
    for(i=0; i<10; i++) {
        sound(random(2000));
        delay(50);
    }
    nosound();
    
    setcolor(YELLOW);
    settextstyle(TRIPLEX_FONT, HORIZ_DIR, 4);
    if (fuel <= 0) {
         outtextxy(180, 80, "OUT OF FUEL!");
    } else {
         outtextxy(200, 80, "GAME OVER");
    }
    
    setcolor(WHITE);
    sprintf(msg, "Final Score: %d", score);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(220, 160, msg);
    
    if (isHighScore) {
        setcolor(LIGHTGREEN);
        outtextxy(180, 200, "NEW HIGH SCORE!");
    } else {
        sprintf(msg, "High Score: %d", highScore);
        setcolor(LIGHTGRAY);
        settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
        outtextxy(220, 200, msg);
    }
    
    setcolor(WHITE);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    outtextxy(200, 260, "Press M for Menu or X to Exit");
    
    while(1) {
        ch = getch();
        if (ch == 'm' || ch == 'M') {
            return 1; // Return to menu
        } else if (ch == 'x' || ch == 'X') {
            return 0; // Exit game
        }
    }
}

void menu() {
    char ch;
    char scoreStr[30];
    
    loadHighScore();
    
    setvisualpage(0);
    setactivepage(0);
    cleardevice();

    setcolor(WHITE);
    settextstyle(TRIPLEX_FONT, HORIZ_DIR, 4);
    outtextxy(220, 50, "CAR RACE");
    
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    setcolor(YELLOW);
    sprintf(scoreStr, "HIGH SCORE: %d", highScore);
    outtextxy(260, 100, scoreStr);

    setcolor(WHITE);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(250, 150, "1. Easy");
    outtextxy(250, 180, "2. Medium");
    outtextxy(250, 210, "3. Hard");
    outtextxy(250, 240, "4. Instructions");
    outtextxy(250, 270, "5. Exit");
    
    outtextxy(200, 320, "Select Mode (1-5):");

    // Engine Start / Menu Sound
    sound(200); delay(100); sound(400); delay(100); nosound();

    while(1) {
        ch = getch();
        if (ch == '1') {
            enemySpeed = 8;
            gameDelay = 50;
            break;
        } else if (ch == '2') {
            enemySpeed = 11;
            gameDelay = 40;
            break;
        } else if (ch == '3') {
            enemySpeed = 14;
            gameDelay = 30;
            break;
        } else if (ch == '4') { // Instructions
            cleardevice();
            outtextxy(50, 50, "INSTRUCTIONS:");
            outtextxy(50, 100, "- Avoid Enemy Cars");
            outtextxy(50, 130, "- Collect Fuel Cans (F) to stay alive");
            outtextxy(50, 160, "- Press P to Pause");
            outtextxy(50, 190, "- Press X to Exit");
            outtextxy(50, 250, "Press any key to return...");
            getch();
            menu(); // Recursive call to simply redraw menu
            return; // Return after recursion to avoid multi-start
        } else if (ch == '5') {
            closegraph();
            exit(0);
        }
    }
}

void main() {
    int playAgain;
    
    setup();
    
    do {
        menu();
        
        // Reset all game elements for a fresh start
        resetGame();
        
        while(!gameOver) {
            draw();
            input();
            logic();
            playMusic();
            delay(gameDelay); 
        }
        
        playAgain = game_over_screen();
        
    } while(playAgain);
    
    closegraph();
}
