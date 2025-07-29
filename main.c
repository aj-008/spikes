#include "raylib.h"
#include "raymath.h"
#include <string.h>

#define SPIKE_HEIGHT 60


static const int screenWidth = 1200;
static const int screenHeight = 800;
Vector2 birdSpeed = { 8, 5 };

const int groundPosY = screenHeight;
const int gravity = 1;


Rectangle topSpikes = { 0, 0, screenWidth, 40 };
Rectangle bottomSpikes = { 0, screenHeight - 40, screenWidth, 40 };
Rectangle spikeArray[10];
int spikeYVals[10];

bool gameOver = false;

//Texture
int numFrames = 0;
int frameWidth = 0;
Rectangle birdFrame = { 0, 0, 0, 0 };
Vector2 birdPosition = { 0, 0 };
unsigned frameCounter = 0;

//Score and colors
unsigned score = 0;
int numColors = 1;
Color spikeColors[1] = { LIGHTGRAY };
int spikePoint[2];
int side[2];
int roundIdx = 0;

//Functions
void InitGame(Texture2D *birdTexture);
static void UpdateGame(Rectangle *birdFrame, Texture2D *birdTexture);
void drawSpikes(void);
void genSpikeCoords(void);

void drawSpikesRound(void);
int spikeCoord = screenHeight/2;




int main(void) {
    InitWindow(screenWidth, screenHeight, "Don't Touch The Spikes");
    Texture2D birdTexture = LoadTexture("flap.png");
    SetTargetFPS(60);

    InitGame(&birdTexture);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        if (!gameOver) {
            UpdateGame(&birdFrame, &birdTexture);
        } else {
            ClearBackground(RAYWHITE);
            char gameOverText[27] = "GAME OVER LOSER\n\nSCORE: ";
            gameOverText[24] = (char)(score / 10) + 48;
            gameOverText[25] = (char)(score % 10) + 48;
            DrawText(gameOverText, 100, 250, 100, RED);
            if (IsKeyPressed(KEY_ENTER)) {
                gameOver = false;
                score = 0;
                birdSpeed.x = 8;
                birdSpeed.y = 5;
                InitGame(&birdTexture);
            }
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}


void InitGame(Texture2D *birdTexture) {
    numFrames = 2;
    frameWidth = birdTexture->width / numFrames;
    birdFrame.width = frameWidth;
    birdFrame.height = birdTexture->height;
    birdPosition.x = (float)screenWidth/4;
    birdPosition.y = (float)screenHeight/4;
    spikePoint[0] = screenWidth - 40;
    spikePoint[1] = 40;
    side[0] = screenWidth;
    side[1] = 0;
    genSpikeCoords();
    drawSpikesRound();
}

static void UpdateGame(Rectangle *birdFrame, Texture2D *birdTexture) {
    DrawRectangleRec(topSpikes, RED);
    DrawRectangleRec(bottomSpikes, RED);


    ClearBackground(RAYWHITE);
    DrawFPS(100, 100);
    char firstDigit = (score / 10) + 48;
    char secondDigit = (score % 10) + 48;
    const char scoreToWrite[2] = { firstDigit, secondDigit };
    DrawText(scoreToWrite, screenWidth/2, screenHeight/2, 200, BLUE);


    DrawTextureRec(*birdTexture, *birdFrame, birdPosition, WHITE);
    birdPosition = Vector2Add(birdPosition, birdSpeed); 
    Rectangle bird = { birdPosition.x, birdPosition.y, (float)frameWidth, 100 };

    drawSpikesRound();

    if (IsKeyPressed(KEY_SPACE)) {
        frameCounter++;
        frameCounter %= numFrames;
        birdFrame->x = frameWidth * frameCounter;
        birdSpeed.y = -15;
    }
    if (IsKeyReleased(KEY_SPACE)) {
        frameCounter++;
        frameCounter %= numFrames;
        birdFrame->x = frameWidth * frameCounter;
    }

    for (int i = 0; i < 10; i++) {
        if (CheckCollisionRecs(spikeArray[i], bird)) {
            gameOver = true;
        }
    }

    if (birdPosition.x >= screenWidth - frameWidth || birdPosition.x <= 0) {
        birdSpeed.x = -birdSpeed.x;
        score += 1;
        roundIdx = score % 2;

        genSpikeCoords();        

    }

    //Collision Check
    DrawRectangleRec(bird, GREEN);

    if (CheckCollisionRecs(topSpikes, bird) || CheckCollisionRecs(bottomSpikes, bird)) {
        gameOver = true;
    }


    birdSpeed.y += gravity;
}

void drawSpikes(void) {
    int spikeIdx = (score % 5) % numColors;
    for (int i = 0; i < screenWidth; i+= 60) {
        DrawTriangle((Vector2){i + 60, 0}, (Vector2){i, 0}, (Vector2){i + 30, 40}, spikeColors[spikeIdx]);
    }
    for (int i = 0; i < screenWidth; i+= 60) {
        DrawTriangle((Vector2){i + 60, screenHeight}, (Vector2){i+30, screenHeight - 40}, (Vector2){i, screenHeight}, spikeColors[spikeIdx]);
    }

}


void drawSpikesRound(void) {
    //generate a rectangle for a spike and place in array of rectangles
    //pick a y poisiton and mod by 60
    //make a rectangle for it
    //will need collision checks for each spike?
    int spikeIdx = (score % 5) % numColors;
    for (int i = 0; i < 10; i++) {
        Rectangle spike_t = { side[roundIdx], spikeYVals[i], 40, 60 };
        spikeArray[i] = spike_t;
        if (spikeYVals[i] != 0) {
            int spikeY = spikeYVals[i];
            int s = side[roundIdx];

            if (roundIdx == 1) {
                DrawTriangle((Vector2){s, spikeY + 60}, (Vector2){spikePoint[roundIdx], spikeY + 30}, (Vector2){s, spikeY}, spikeColors[spikeIdx]);
            } else {
                DrawTriangle((Vector2){s, spikeY + 60}, (Vector2){s, spikeY}, (Vector2){spikePoint[roundIdx], spikeY + 30}, spikeColors[spikeIdx]);
            }
        }
    }

    //top and bottom
    for (int i = 0; i < screenWidth; i+= 60) {
        DrawTriangle((Vector2){i + 60, 0}, (Vector2){i, 0}, (Vector2){i + 30, 40}, spikeColors[spikeIdx]);
    }
    for (int i = 0; i < screenWidth; i+= 60) {
        DrawTriangle((Vector2){i + 60, screenHeight}, (Vector2){i+30, screenHeight - 40}, (Vector2){i, screenHeight}, spikeColors[spikeIdx]);
    }
}

void genSpikeCoords(void) {
    for (int i = 0; i < 10; i ++) {
        spikeYVals[i] = 0;
        int check = GetRandomValue(0, 1);
        if (check) {
            spikeYVals[i] = i * SPIKE_HEIGHT;
        }
    }
}

