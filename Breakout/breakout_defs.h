/* --------------------------------------------------------
 *    File: breakout_defs.h
 *  Author: Leila Alteme
 *   Class: COP 2001 CRN 10410
 * Purpose: Header File for 2D Breakout Arcade Game
 * 04.26.22 LA created file
 * -------------------------------------------------------- */
#include <SFML/Graphics.hpp>

#ifndef BREAKOUTFINAL_BREAKOUT_DEFS_H
#define BREAKOUTFINAL_BREAKOUT_DEFS_H

// Global Constants
// --------------------------------------------------------


// window properties
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const sf::Color WINDOW_COLOR = sf::Color::White;

// drawing properties
const float FRAME_RATE = (1.0/30.0) * 1000.0;   // FPS in ms
const sf::Color BALL_COLOR = sf::Color::Yellow;
const float WALL_THICKNESS = 5.0;
const sf::Color WALL_COLOR = sf::Color::Blue;

// paddle properties
const float PADDLE_WIDTH = 80.0;
const float PADDLE_THICKNESS = 10.0;
const sf::Color PADDLE_COLOR = sf::Color:: Red;
const float PADDLE_SPEED = PADDLE_WIDTH / 10.0 / 1000.0;

// brick constants
const int BRICK_ROWS = 8;
const int BRICK_COLUMNS = 14; // rows of bricks in rack
const float BRICK_WIDTH = WINDOW_WIDTH / BRICK_COLUMNS; // number of bricks in row
const float BRICK_HEIGHT = PADDLE_THICKNESS * 2; // width of 1 brick
const float BRICKS_HEIGHT = BRICK_ROWS * BRICK_HEIGHT; // height of rack of bricks
const float BRICKS_TOP = WINDOW_HEIGHT / 2.0 - BRICK_HEIGHT * 0.75; // top of the rack (shift up 3/4 from center)
const float BRICKS_LEFT = WALL_THICKNESS; // left of first brick in each row
const float FIRST_BRICK = BRICKS_TOP +(BRICK_ROWS - 1) * BRICK_HEIGHT; //top of the lowest brick in rack

// Type definitions
// --------------------------------------------------------
enum Direction {
    Exit = -1,
    None = 0,
    Left = 1,
    Up = 2,
    Right = 3,
    Down = 4,
    Start,
};

struct Ball {
    float radius;
    float coordinateX;
    float coordinateY;
    float velocityX;
    float velocityY;
    sf::Color color;
};

struct Block {
    float left;
    float top;
    float width;
    float height;
    sf::Color color;
    sf::RectangleShape rectangle;
};

struct MovingBlock {
    Block properties;
    float X_Velocity;
    float Y_Velocity;
};

struct Borders {
    Block leftWall;
    Block topWall;
    Block rightWall;
    Block bottomWall;
};

struct Brick {
    Block block;
    bool hit;
    int points;
    float speedAdjust;
};

// These are just for fun
// speed that the can accelerate at to span window in
// n-seconds (in ms) broken up for each frame
const float SPEED_TIME = (3.0 * 1000.0) * 30.0;     //
const float BALL_RADIUS = 10.0;
const float BALL_SPEED_X = BALL_RADIUS * 10.0 / 1000.0;    // speed horizontally
const float BALL_SPEED_Y = BALL_RADIUS * 8.5 / 1000.0;   // span  vertically


#endif //BREAKOUTFINAL_BREAKOUT_DEFS_H
