#include "Common.h"

/*Consts*/
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 400;
const char* TITLE = "BREAKOUT - A completely orignal idea";
const int BRICK_ROW = 11; //-
const int BRICK_COLOMN = 5; // |

/*Declarations*/
//--Walls
GameObject leftWall(0, 0, 20, SCREEN_HEIGHT, true, BLUE);
GameObject rightWall(SCREEN_WIDTH - 20, 0, 20, SCREEN_HEIGHT, true, BLUE);
GameObject topWall(0, 0, SCREEN_WIDTH, 20, true, BLUE);
//--Paddle
Paddle paddle(SCREEN_WIDTH / 2 - 20, SCREEN_HEIGHT - 20, 50, 10, true, RED);
//--Ball
Ball ball(paddle.position.x + 25, paddle.position.y - 10, 5, true, RED);
//--Bricks
Brick brick[BRICK_COLOMN][BRICK_ROW];

/*Global variables*/
GameState currentState = MAIN_MENU;
int g_score = 0;
int g_highScore = 0;
Vector2 g_originalBallPos = ball.position; //The ball's original position in case of a reset
Vector2 g_originalPaddlePos = paddle.position; //Same for the paddle
bool g_isPaused = false;

/*Function signutres*/
//--Makes the window
void MakeWindow();
//Main menu screen
void MainMenu();
//Pause screen
void PauseScreen();
//Game over screen
void GameOverScreen();
//Resets the game
void ResetGame();

/*Main function*/
int main()
{
    /*Making the window*/
    MakeWindow();

    //Initial brick position, which will get added to every brick 
    float g_posX = 30;
    float g_posY = 40;

    //Setting the blocks' positions
    for (int i = 0; i < BRICK_COLOMN; i++)
    {
        for (int j = 0; j < BRICK_ROW; j++)
        {
            brick[i][j].position = Vector2{g_posX, g_posY};

            g_posX += 50;
        }
        
        g_posX = 30;
        g_posY += 30;
    }

    /*Main game loop*/
    while(!WindowShouldClose())
    {
        /*Movements*/
        //--Paddle movement
        if(paddle.isMoving) paddle.MovePaddle(350.0f);
        //--Ball movement
        if(ball.isMoving) ball.MoveBall();

        /*Clamping movement*/
        paddle.position.x = Clamp(paddle.position.x, leftWall.position.x + paddle.width / 2 - 5, rightWall.position.x - paddle.width);

        /*Collision handaling*/
        //--Collision with the ball and the paddle        
        if(CheckCollisionCircleRec(ball.SetBallCenter(), ball.radius, paddle.SetCollider()))
        {
            ball.position.y = paddle.position.y - ball.radius;
            ball.speedY *= -1.0f;
        }
        //--Collision with the ball and the walls
        if(CheckCollisionCircleRec(ball.SetBallCenter(), ball.radius, leftWall.SetCollider())) 
        {
            ball.position.x = leftWall.position.x + 25;
            ball.speedX *= -1.0f;
        }
        if(CheckCollisionCircleRec(ball.SetBallCenter(), ball.radius, rightWall.SetCollider()))
        {
            ball.position.x = rightWall.position.x - ball.radius;
            ball.speedX *= -1.0f;
        }
        if(CheckCollisionCircleRec(ball.SetBallCenter(), ball.radius, topWall.SetCollider()))
        {
            ball.position.y = topWall.position.y + 25;
            ball.speedY *= -1.0f;
        }
        //--Collision with the ball and the bricks
        for (int i = 0; i < BRICK_COLOMN; i++)
        {
            for (int j = 0; j < BRICK_ROW; j++)
            {
                if(CheckCollisionCircleRec(ball.SetBallCenter(), ball.radius, brick[i][j].SetBrickCollider()))
                {
                    brick[i][j].isActive = false;
                    ball.speedY *= -1.0f;
                    g_score++;
                } 
            } 
        }
        
        /*Score handaling*/
        //Resets the bricks when all of them are hit
        if(g_score % 55 == 0)
        {
            for (int i = 0; i < BRICK_COLOMN; i++)
            {
                for (int j = 0; j < BRICK_ROW; j++)
                {
                    if(!brick[i][j].isActive) brick[i][j].isActive = true; 
                }               
            }
        }
        //Starting the game when SPACE is pressed
        if(IsKeyDown(KEY_SPACE) && currentState == GAMEPLAY) 
        {
            paddle.isMoving = true;
            ball.isMoving = true;
        }
        //Setting the highscore
        if(g_score > g_highScore) g_highScore = g_score;

        /*Game states handaling*/
        //--Main menu to gameplay
        if(IsKeyDown(KEY_ENTER) && currentState == MAIN_MENU) 
        {
            ResetGame();
            currentState = GAMEPLAY;
        }
        //--Gameplay to game over(only when the ball goes out of bounds)
        if(ball.position.y > GetScreenHeight() && currentState == GAMEPLAY) currentState = GAME_OVER;
        //--Pause to gameplay(and then back)
        if(IsKeyPressed(KEY_P) && currentState == GAMEPLAY) g_isPaused = !g_isPaused;
        //--Pause to main menu
        if(IsKeyDown(KEY_M) && g_isPaused) currentState = MAIN_MENU;
        //--Game over to gameplay
        if(IsKeyDown(KEY_R) && currentState == GAME_OVER)
        {
            ResetGame();
            currentState = GAMEPLAY;
        }
        //Stopping the ball and the paddle if the screen is paused
        if(g_isPaused)
        {
            ball.isMoving = false;
            paddle.isMoving = false;
        }
        
        /*Drawing*/
        BeginDrawing();
            ClearBackground(RAYWHITE);

            switch (currentState)
            {
            case MAIN_MENU: MainMenu(); break;
            case GAMEPLAY:
            {
                /*Objects*/
                //--Walls
                if(leftWall.isActive) leftWall.Spawn();
                if(rightWall.isActive) rightWall.Spawn();
                if(topWall.isActive) topWall.Spawn();
                //--Blocks
                for (int i = 0; i < BRICK_COLOMN; i++)
                {
                    for (int j = 0; j < BRICK_ROW; j++)
                    {
                        if(brick[i][j].isActive) brick[i][j].SpawnBrick();
                    }
                }
                //--Paddle
                if(paddle.isActive) paddle.Spawn();
                //--Ball
                if(ball.isActive) ball.SpawnBall();

                /*UI*/
                //--Text
                DrawText(TextFormat("Score: %i", g_score), GetScreenWidth() / 2 - 50, 2, 25, BLACK);
            }
            break;
            case GAME_OVER: GameOverScreen(); break;
            }

            if(g_isPaused && currentState == GAMEPLAY) PauseScreen();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

/*Functions*/
void MakeWindow()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
    SetWindowState(FLAG_VSYNC_HINT);
}

void ResetGame()
{
    ball.position = g_originalBallPos;
    ball.isMoving = false;
    paddle.position = g_originalPaddlePos;
    paddle.isMoving = false;

    g_score = 0;

    for (int i = 0; i < BRICK_COLOMN; i++)
    {
        for (int j = 0; j < BRICK_ROW; j++)
        {
            if(!brick[i][j].isActive) brick[i][j].isActive = true;
        }
    }
}

void MainMenu()
{
    const char* mainTitle = "BREAKOUT";
    int mainTextWidth = MeasureText(mainTitle, 50);
    //Main title
    DrawText(mainTitle, GetScreenWidth() / 2 - mainTextWidth / 2, 10, 50, BLUE);
    DrawLineEx(Vector2{(float)mainTextWidth + 160, 60}, Vector2{(float)mainTextWidth - 120, 60}, 5, BLUE);
    //Play text
    const char* playText = "[ENTER] to play";
    int playTextWidth = MeasureText(playText, 30);
    DrawText(playText, GetScreenWidth() / 2 - playTextWidth / 2, GetScreenHeight() / 2, 30, BLUE);
    DrawRectangleLines(GetScreenWidth() / 2 - 135, GetScreenHeight() / 2 - 10, 270, 50, BLUE);
    //Exit text
    DrawText("[ESC] to exit", 5, GetScreenHeight() - 23, 20, BLUE);

    g_isPaused = false;
}

void PauseScreen()
{
    //Pause text
    const char* pauseText = "PAUSE";
    int textWidth = MeasureText(pauseText, 30);
    DrawText(pauseText, GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() / 2 - 10, 30, BLUE);
    //To main menu text
    DrawText("[M] to go to the Main Menu", 20, GetScreenHeight() - 23, 20, BLUE);
}

void GameOverScreen()
{
    //Game over text
    const char* gameOverText = "GAME OVER!";
    int gameOverTextWidth = MeasureText(gameOverText, 50);
    DrawText(gameOverText, GetScreenWidth() / 2 - gameOverTextWidth / 2, 30, 50, BLUE);
    //Score text
    DrawText(TextFormat("Score: %i", g_score), GetScreenWidth() / 2 - 55, GetScreenHeight() / 2 - 70, 30, BLUE);
    //High score text
    DrawText(TextFormat("High score: %i", g_highScore), GetScreenWidth() / 2 - 70, GetScreenHeight() / 2 - 30, 25, BLUE);
    //Replay text
    DrawText("[R] to replay", 5, GetScreenHeight() - 23, 20, BLUE);

    g_isPaused = false;
}
