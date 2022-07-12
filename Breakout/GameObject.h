#pragma once

#include "raylib.h"

/*Const*/
const float BLOCK_WIDTH = 40;
const float BLOCK_HEIGHT = 10;

/*Classes*/
class GameObject
{
public:
    Vector2 position;
    float width, height;
    bool isActive;
    Color color;

public:
    GameObject(float xPos, float yPos, float width, float height, bool active, Color color)
        :position(Vector2{xPos, yPos}), width(width), height(height), isActive(active), color(color)
    {
    }  

public:
    Rectangle SetCollider()
    {
        return Rectangle{position.x, position.y, width, height};
    }   

    void Spawn()
    {
        DrawRectangle(position.x, position.y, width, height, color);
    }   
};

class Paddle : public GameObject
{
public:
    using GameObject::GameObject;

public:
    bool isMoving = false;

public:
    void MovePaddle(const float&& moveSpeed)
    {
        if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) position.x -= moveSpeed * GetFrameTime();
        if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) position.x += moveSpeed * GetFrameTime();
    }
};

class Ball
{
public:
    Ball(float xPos, float yPos, float radius, bool active, Color color)
        :position(Vector2{xPos, yPos}), radius(radius), isActive(active), color(color)
    {
        isMoving = false;
        speedX = 150;
        speedY = 300;
    }

public: 
    Vector2 position;
    float radius;
    float speedX, speedY;
    bool isActive, isMoving;
    Color color;

public:
    Vector2 SetBallCenter()
    {
        return Vector2{position.x, position.y};
    }  

    void SpawnBall()
    {
        DrawCircleV(SetBallCenter(), radius, color);
    }  

    void MoveBall()
    {
        position.x += speedX * GetFrameTime();
        position.y += speedY * GetFrameTime();
    }
};

class Brick
{
public:
    Vector2 position;
    float width = BLOCK_WIDTH; 
    float height = BLOCK_HEIGHT;
    Color color = BLUE;
    bool isActive = true;

public:  
    Rectangle SetBrickCollider()
    {
        //Setting the rectangle only if the brick is active
        //Doing this to avoid the invinsible collision glitch
        if(isActive) return Rectangle{position.x, position.y, width, height};
        else return Rectangle{0, 0, 0, 0};
    }

    void SpawnBrick()
    {
        DrawRectangle(position.x, position.y, width, height, color);
    } 
};