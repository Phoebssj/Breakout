/* --------------------------------------------------------
 *    File: linearcollisions.cpp
 *  Author: Leila Alteme
 *   Class: COP 2001 CRN 10410
 * Purpose: 2D Breakout Arcade Game
 * 3.30.22 LA added walls to main app file
 * 4.08.22 LA added collision detection to main app file
 * 4.19.22 LA updated logic in main app file
 * 4.26.22 LA added paddle breakout to mail app file
 * 4.26.22 LA added header file
 * 5.03.22 LA added bricks to main app file
 * -------------------------------------------------------- */
#include <cmath>       // Trig and Linear Functions
#include "breakout_defs.h"



// Function declarations (prototypes)
// --------------------------------------------------------
void setup(Ball &ball, Borders &walls, MovingBlock &paddle, Brick theBricks[BRICK_ROWS][BRICK_COLUMNS]);
Direction processInput() ;
bool update(Direction &input, Ball &ball, Borders walls, float delta, MovingBlock &paddle, bool &started, Brick theBricks[BRICK_ROWS][BRICK_COLUMNS]);
void render(sf::RenderWindow &window, Ball ball, Borders theWalls, float delta, MovingBlock paddle, Brick theBricks[BRICK_ROWS][BRICK_COLUMNS]);
int getCollisionPoint (Ball* pBall, Block* pBlock);
bool checkCollision (Ball* pBall, Block* pBlock);
bool checkBlockCollision(Block paddle, Block walls);
bool doCollisionChecks(Ball &ball, MovingBlock &paddle, Borders walls, Brick theBricks[BRICK_ROWS][BRICK_COLUMNS]);

/**
 * The main application
 * @return OS stats message (0=Success)
 */
int main(){

    // create a 2d graphics window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Hello SFML");
    window.clear(WINDOW_COLOR);

    Ball theBall;
    Borders theWalls;
    MovingBlock paddle;
    Brick theBricks[BRICK_ROWS][BRICK_COLUMNS];
    setup(theBall, theWalls, paddle, theBricks);


    // timing variables for the main game loop
    sf::Clock clock;
    sf::Time startTime = clock.getElapsedTime();
    sf::Time stopTime = startTime;
    float delta = 0.0;

    bool started = false;
    bool gameOver = false;
    while (!gameOver)
    {
        // calculate frame time
        stopTime = clock.getElapsedTime();
        delta += (stopTime.asMilliseconds() - startTime.asMilliseconds());
        startTime = stopTime;

        // process events
        sf::Event event;
        while (!gameOver && window.pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                gameOver = true;
        }

        // Process user input
        // ------------------------------------------------
        Direction userInput = processInput();
        if (userInput == Exit)
            gameOver = true;

        // Process Updates
        // ------------------------------------------------
        if (delta >= FRAME_RATE) {    // if we have made it at least a full frame time

            gameOver = update(userInput, theBall, theWalls, delta, paddle, started, theBricks);

            // subtract the frame-rate from the current frame-time
            // for each full frame covered by this update
            while (delta >= FRAME_RATE)
                delta -= FRAME_RATE;
        }

        // Render the window
        // ------------------------------------------------
        render(window, theBall, theWalls, delta, paddle, theBricks);
    } // end main game loop

    // game ending, close the graphics window
    window.close();

    return 0;   // return success to the OS
} // end main

/**
 *@param walls - structure variable with properties for the walls
 * @param ball - update ball position and speed
 * @param paddle - structure variable with properties for the paddle
 * @param bricks - structure variable with properties for the bricks
 */
void setup(Ball &ball, Borders &walls, MovingBlock &paddle, Brick theBricks[BRICK_ROWS][BRICK_COLUMNS]){

    walls.leftWall.left = 0.0;
    walls.leftWall.top = 0.0;
    walls.leftWall.width = WALL_THICKNESS;
    walls.leftWall.height = WINDOW_HEIGHT;
    walls.leftWall.color = WALL_COLOR;
    walls.leftWall.rectangle.setSize(sf::Vector2(walls.leftWall.width, walls.leftWall.height));
    walls.leftWall.rectangle.setPosition(walls.leftWall.left, walls.topWall.top);
    walls.leftWall.rectangle.setFillColor(WALL_COLOR);

    walls.topWall.left = 0.0;
    walls.topWall.top = 0.0;
    walls.topWall.width = WINDOW_WIDTH;
    walls.topWall.height = WALL_THICKNESS;
    walls.topWall.color = WALL_COLOR;
    walls.topWall.rectangle.setSize(sf::Vector2(walls.topWall.width, walls.topWall.height));
    walls.topWall.rectangle.setPosition(walls.topWall.left, walls.topWall.top);
    walls.topWall.rectangle.setFillColor(WALL_COLOR);

    walls.rightWall.left = WINDOW_WIDTH - WALL_THICKNESS;
    walls.rightWall.top = 0.0;
    walls.rightWall.width = WALL_THICKNESS;
    walls.rightWall.height = WINDOW_HEIGHT;
    walls.rightWall.color = WALL_COLOR;
    walls.rightWall.rectangle.setSize(sf::Vector2(walls.rightWall.width, walls.rightWall.height));
    walls.rightWall.rectangle.setPosition(walls.rightWall.left, walls.topWall.top);
    walls.rightWall.rectangle.setFillColor(WALL_COLOR);

    walls.bottomWall.left = 0.0;
    walls.bottomWall.top = WINDOW_HEIGHT - WALL_THICKNESS;
    walls.bottomWall.width = WINDOW_WIDTH;
    walls.bottomWall.height = WALL_THICKNESS;
    walls.bottomWall.color = WALL_COLOR;
    walls.bottomWall.rectangle.setSize(sf::Vector2(walls.bottomWall.width, walls.bottomWall.height));
    walls.bottomWall.rectangle.setPosition(walls.bottomWall.left, walls.bottomWall.top);
    walls.bottomWall.rectangle.setFillColor(WALL_COLOR);

    // initialize paddle parameter
    paddle.properties.left = (WINDOW_WIDTH - PADDLE_WIDTH) / 2.0;
    paddle.properties.top = walls.bottomWall.top - ( 2.0 * PADDLE_THICKNESS);
    paddle.properties.width = PADDLE_WIDTH;
    paddle.properties.height = PADDLE_THICKNESS;
    paddle.properties.color = PADDLE_COLOR;
    paddle.properties.rectangle.setSize(sf::Vector2(paddle.properties.width, paddle.properties.height));
    paddle.properties.rectangle.setFillColor(PADDLE_COLOR);
    paddle.X_Velocity = 0;
    paddle.Y_Velocity = 0;

    // declare a ball variable and populate it in the center of the window
    ball.radius = 10.0;
    ball.coordinateX = paddle.properties.left + (PADDLE_WIDTH / 2.0);
    ball.coordinateY = paddle.properties.top - ball.radius - 1;
    ball.velocityX = 0.0;
    ball.velocityY = 0.0;
    ball.color = BALL_COLOR;

    float bricksTop = FIRST_BRICK; // start at lowest brick row

    Brick* pNext = &theBricks[0][0]; //get pointer to 1st brick
    for (int row = 0; row < BRICK_ROWS; row++){

        float bricksLeft = BRICKS_LEFT; // start back at far left

        for (int column = 0; column < BRICK_COLUMNS; column++){
            pNext ->block.left = bricksLeft + 1; // 1 pixel right to leave blank pixel on left

            // offset left/top by 1 pixel to give each brick a border
            pNext->block.left = bricksLeft + 1;
            pNext->block.top = bricksTop + 1;

            //subtract 2 from width/height to make room for the 1-pixel
            //border on each side
            pNext->block.width = BRICK_WIDTH - 2;
            pNext->block.height = BRICK_HEIGHT - 2;

            // set row specific properties
            if (row < 2){
                pNext->block.color = sf::Color::Yellow;
                pNext->points = 1;
                pNext->speedAdjust = 0;
            }
            else if (row < 4){
                pNext->block.color = sf::Color::Green;
                pNext->points = 3;
                pNext->speedAdjust = 0;
            }
            else if (row < 6){
                pNext->block.color = sf::Color(255,165,0); //Orange
                pNext->points = 5;
                pNext->speedAdjust = 1;
            }
            else{
                pNext->block.color =sf::Color::Red;
                pNext->points = 7;
                pNext->speedAdjust = 2;
            }

            pNext++; // get next brick

            bricksLeft += BRICK_WIDTH; //move next brick to right

        } // brick columns

        bricksTop -= BRICK_HEIGHT; // move up to next row
    } // brick rows

    // set the drawing rectangle
    pNext->block.rectangle.setSize(sf::Vector2f(pNext->block.width, pNext->block.height));
    pNext->block.rectangle.setFillColor(pNext->block.color);
    pNext->block.rectangle.setPosition(pNext->block.left, pNext->block.top);

    //initialize hit flag off so brick is displayed
    pNext->hit = false;
} // end setup


/**
 * convert user keyboard input into recognized integer values
 * for left=1/up=2/right=3/down=4
 * @return int - user input (default no-input=0, quit=-1)
 */
Direction processInput() {
    Direction input = None;  // no input

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        input = Left;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        input = Up;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        input = Right;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        input = Down;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
        input = Exit;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
        input = Start;
    }

    return input;
} // end getUserInput

/**
 *
 * @param pBall - with coordinates and radius
 * @param pBlock - with location and width
 */

int getCollisionPoint(Ball* pBall, Block* pBlock) {
    int collision = 0;
    float checkPointX;
    float checkPointY;

    // -- find right horizontal coordinate
    // -- ball left of block --
    if (pBall->coordinateX < pBlock->left) {
        checkPointX = pBlock->left;
    }
        // -- ball right of block --
    else if (pBall->coordinateX > pBlock->left + pBlock->width) {
        checkPointX = pBlock->left + pBlock->width;
    }
        // -- ball between block --
    else {
        checkPointX = pBall->coordinateX;
    }
    // find vertical coordinate
    // -- ball above of block --
    if (pBall->coordinateY < pBlock->top) {
        checkPointY = pBlock->top;
    }
        // -- ball below of block --
    else if (pBall->coordinateY > pBlock->top + pBlock->height) {
        checkPointY = pBlock->top + pBlock->height;
    }
        // -- ball between block --
    else {
        checkPointY = pBall->coordinateY;
    }

    float diffX = checkPointX - pBall->coordinateX;
    float diffY = (WINDOW_HEIGHT - checkPointY) - (WINDOW_HEIGHT - pBall->coordinateY);

    double distance = std::sqrt(pow(diffX, 2.0) + pow(diffY, 2.0));

    if (distance <= pBall->radius) {
        double theta = std::atan2(diffY, diffX);

        double degrees = 90.0 - theta * 180 / M_PI;

        if (degrees <= 0)
            degrees += 360;
        collision = int(degrees);
    }

    return collision;
}  //getCollisionPoint


bool checkCollision (Ball* pBall, Block* pBlock){
    bool hasCollided = false;

    int heading = getCollisionPoint(pBall, pBlock);

    // -- if collision detected --
    if (heading){
        hasCollided = true;

        // check for horizontal collision
        if (heading > 225 && heading < 315){ // hit left of ball
            pBall -> velocityX *= -1;
            pBall -> coordinateX = pBlock -> left + pBlock -> width + pBall -> radius + 1;
        }
        else if (heading > 45 && heading < 135) { // hit right of ball
            pBall->velocityX *= -1;
            pBall->coordinateX = pBlock->left - pBall->radius - 1;
        }

        //check vertical collision
        if (heading >= 315 || heading <= 45){ // top of ball
            pBall -> velocityY *= - 1;
            pBall -> coordinateY = pBlock -> top + pBlock -> height + pBall -> radius + 1;
        }
        else if (heading >= 135 && heading <= 225){ // bottom of ball
            pBall -> velocityY *= - 1;
            pBall -> coordinateY = pBlock -> top - pBall -> radius - 1;
        }
    }
    return hasCollided;
}
//checkCollision

bool checkBlockCollision(Block paddle, Block walls) {
    bool blockCollision = false;
    if (paddle.left < walls.left + walls.width && paddle.left + paddle.width > walls.left &&
        paddle.top < walls.top + walls.height && paddle.top + paddle.height > walls.top) {
        blockCollision = true;
    }

    return blockCollision;
}

bool doCollisionChecks(Ball &ball, MovingBlock &paddle, Borders walls, Brick theBricks[BRICK_ROWS][BRICK_COLUMNS]){
    bool gameOver = false;

    //--check vertical collisions--
    if (!checkCollision(&ball, &paddle.properties)) {
        if (!checkCollision(&ball, &walls.topWall)) {
            gameOver = checkCollision(&ball, &walls.bottomWall);
        }
    }
    // --- check horizontal collisions ---
    if (!checkCollision(&ball, &walls.leftWall)) {
        checkCollision(&ball, &walls.rightWall);
    }

    //--check paddle and wall collisions --
    float paddleX_Coordinate;
    if (checkBlockCollision(paddle.properties, walls.leftWall)) {
        paddle.X_Velocity = 0.0;
        paddleX_Coordinate = walls.leftWall.left + walls.leftWall.width + 1.0;
    } else if (checkBlockCollision(paddle.properties, walls.rightWall)) {
        paddle.X_Velocity = 0.0;
        paddleX_Coordinate = walls.rightWall.left - paddle.properties.width - 1.0;
    }

    Brick *pBrick = &theBricks[0][0];
    for (int row = 0; row < BRICK_ROWS; row){
        for (int column = 0; column < BRICK_COLUMNS; column++){
            if (!pBrick->hit){
                pBrick->hit = checkCollision(&ball, &pBrick->block);
            }
            pBrick++;
        } // columns
    } // rows

    return gameOver;
};//doCollisionChecks

/**
 * update the state of game objects
 * @param input - user keyboard input
 * @param ball  - update ball position and speed
 * @param walls - structure variable with properties for the walls
 * @param delta - current frame time
 * @param started - variable for start of game
 * @param paddle - structure variable with properties for the paddle
 * @param doCollisionChecks - general function for collision checks
 */
bool update(Direction &input, Ball &ball, Borders walls, float delta, MovingBlock &paddle, bool &started, Brick theBricks[BRICK_ROWS][BRICK_COLUMNS]) {
    bool gameOver = false;
    // adjust velocity directions for user input
    if (input) {
        switch (input) {
            case Left: // Left
                paddle.X_Velocity -= PADDLE_SPEED;
                break;
            case Up: // Up
                paddle.Y_Velocity -= PADDLE_SPEED;
                break;
            case Right: // Right
                paddle.X_Velocity += PADDLE_SPEED;
                break;
            case Down: // Down
                paddle.Y_Velocity += PADDLE_SPEED;
                break;
            case Start:
                if (!started) {
                    ball.velocityX = BALL_SPEED_X;
                    ball.velocityY = BALL_SPEED_Y * -1; // start moving upwards
                    started = true;
                }
        }
        // clear input
        input = None;
    }

    if (started) {
        // adjust the location of the ball for speed * time
        paddle.properties.left += paddle.X_Velocity * delta;
        ball.coordinateY += ball.velocityY * delta;
    } else {
        ball.coordinateX = paddle.properties.left + (PADDLE_WIDTH / 2.0);
        ball.coordinateY = paddle.properties.top - ball.radius - 1;
    }
    gameOver = doCollisionChecks(ball, paddle, walls, theBricks);
    return gameOver;
}// end update



/**
 * draw the ball on the graphics window
 * @param window - handle to open graphics window
 * @param ball   - structure variable with properties for the ball
 * @param delta  - amount of frame time plus lag (in ms)
 * @param theWalls - update properties for borders
 * @param paddle - structure variable with properties for the paddle
 * @param bricks - structure variable with properties for the bricks
 */
void render(sf::RenderWindow &window, Ball ball, Borders theWalls, float delta, MovingBlock paddle, Brick theBricks[BRICK_ROWS][BRICK_COLUMNS]) {
    // Render drawing objects
    // ------------------------------------------------
    // clear the window with the background color
    window.clear(WINDOW_COLOR);

    // draw the ball
    // ------------------------------------------------
    sf::CircleShape circle;
    circle.setFillColor(ball.color);
    circle.setRadius(ball.radius);
    // set screen coordinates relative to the center of the circle
    circle.setOrigin(ball.radius, ball.radius);
    // calculate current drawing location relative to speed and frame-time
    float xPosition = ball.coordinateX + ball.velocityX * delta;
    float yPosition = ball.coordinateY + ball.velocityY * delta;
    circle.setPosition(xPosition, yPosition);
    window.draw(circle);

    window.draw(theWalls.leftWall.rectangle);
    window.draw(theWalls.topWall.rectangle);
    window.draw(theWalls.rightWall.rectangle);
    window.draw(theWalls.bottomWall.rectangle);

    // render of paddle
    paddle.properties.left += paddle.X_Velocity;
    paddle.properties.top += paddle.Y_Velocity;
    paddle.properties.rectangle.setPosition(paddle.properties.left, paddle.properties.top);
    window.draw(paddle.properties.rectangle);

    // render of bricks
    Brick *pBrick = &theBricks[0][0];
    for (int row = 0; row < BRICK_ROWS; row++){
        for (int column = 0; column < BRICK_COLUMNS; column++){
            if (!pBrick->hit){
                window.draw(pBrick->block.rectangle);
            }
            pBrick++;
        } // columns
    } // rows

    // show the new window
    // ------------------------------------------------
    window.display();
} // end render
