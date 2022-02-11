#include "GhostBuster.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

// Currently rows and columns are set to 8, however your game implementation should work for any other number
const int rows = 9, cols = 9;

// grid will store characters matrix of rows*cols size, you have to allocate memory to it in initialize function below
char *grid = NULL;

// Ghost will be placed over this location
int ghostRow, ghostCol;

// *****************************************************************************************************
// No change zone: Don't change anything until the next stars line
// *****************************************************************************************************

// Don't Change it: Screen width and height values
const int width = 1000, height = 600;

// Don't Change: This function is dealing with SDL and currently it's complete. Don't change anything here
void drawOneBlock(SDL_Renderer *renderer, SDL_Texture *texture, int row, int col, char sym)
{
    int xbox = width / cols;
    int ybox = height / rows;
    SDL_Rect src;
    switch (sym)
    {
    case 'L':
        src = {141, 7, 104, 113};
        break;
    case 'S':
        src = {427, 12, 129, 103};
        break;
    case 'T':
        src = {268, 10, 130, 111};
        break;
    case 'B':
        src = {0, 3, 114, 111};
        break;
    case 'G':
        src = {571, 0, 123, 121};
        break;
    case 'Z':
        src = {571, 0, 25, 25};
        break;
    case 'F':
        src = {707, 16, 90, 86};
        break;
    }
    SDL_Rect mov = {xbox * col, ybox * row, xbox - 10, ybox - 10};
    SDL_RenderCopy(renderer, texture, &src, &mov);
}

void drawBlocks(SDL_Renderer *renderer, SDL_Texture *texture)
{
    // Call drawOneBlock for all of the blocks of grid
    // for example to draw a snake over block (3, 5) you call it this way:
    // drawOneBlock(renderer, texture, 3, 5, 'S');
    if (grid == NULL)
        return;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            drawOneBlock(renderer, texture, i, j, grid[i * cols + j]);
        }
    }
}

// *****************************************************************************************************
// No change zone ends here
// *****************************************************************************************************

// To Do zone:

// ! Use "Better Comments" extension for better comments :)

// * ----------------- Defining variables

// ? variables defined above
/**
 * * ghostCol - for assigning the column for the ghost
 * * ghostRow - for assigning the row for the ghost
 * * *grid -  for declaring dynamic array in the memory on which the game runs
 * ! constants
 * * rows - to refine number of rows for the game
 * * cols -  to define number of columns for the game
 * * width - to define the width of the window
 * * height - to define the height of the window
 *
 */

// ? bool ended variable to stop the game
// * - false means the game is running.
// * - true means the game is lost/won.
bool ended = false;

// * ----------------- Helper functions ----------------

/**
 * ? @brief checkCoOrdinates function converts x and y pixels to the index of the block
 * * according to the grid[] initialized above
 * * also it saves the block width and height nad locationX and locationY
 * ? locationX is the row and locationY is col of the clicked block
 *
 * ? @param x
 * * int x pixel of the screen
 * ? @param y
 * * int y pixel of the screen
 *
 * ! @return int
 * * index of grid[] or the number of clicked block
 */
int checkCoOrdinates(int x, int y)
{
    int blockWidth = width / cols;
    int blockHeight = height / rows;
    // cout << "Block Size: " << blockWidth << ", " << blockHeight << endl;
    int locationX = (x) / blockWidth;
    int locationY = (y) / blockHeight;
    // cout << "location: " << locationX << ", " << locationY << endl;

    return (locationY * cols) + locationX;
}

/**
 * ? @brief getGhostBlock gets the Ghost Block index for the grid[]
 *
 * ? @param ghostX
 * * int row for the ghost
 * ? @param ghostY
 * * int col for the ghost
 *
 * ! @return int
 * * index of ghost
 */

int getGhostBlock(int ghostX, int ghostY)
{
    return (ghostY * cols) + ghostX;
}

/**
 * ? brief generateRandomInteger gived random number according to range given
 *
 * ? @param min
 * * int - the lowest number possible
 * ? @param max
 * * int - the highest number possible
 *
 * ! @return int
 * * random int
 */

int generateRandomInteger(int min, int max)
{
    int range = max - min + 1;
    return rand() % range + min;
}

/**
 * ? @brief generateRandomChar gives a random char
 * * by getting the random integer from generateRandomInteger function
 *
 * ! @return char
 * * if random int is 1
 * * -- returns S
 * * if it is 2
 * * -- returns T
 * * if it is 3
 * * -- returns B
 * * else
 * * -- returns S
 */

char generateRandomChar()
{
    int random = generateRandomInteger(1, 3);
    switch (random)
    {
    case 1:
        return 'S';
    case 2:
        return 'T';
    case 3:
        return 'B';
    default:
        return 'S';
    }
}
/**
 * ? @brief distanceCalculator takes 2 points (x, y)
 * * and caluclates the distance between the two points
 *
 * ? @param cX
 * * int - x co-ordinate for clickedBlock
 * ? @param cY
 * * int - y co-ordinate for clickedBlock
 * ? @param gX
 * * int - x co-ordinate for ghostBlock
 * ? @param gY
 * * int - y co-ordinate for ghostBlock
 *
 * ! @return double
 * * distance of two points - decimal number
 */
double distanceCalculator(int cX, int cY, int gX, int gY)
{
    return sqrt(pow((cX - gX), 2) + pow((cY - gY), 2));
}

/**
 * ? @brief fillBlock gets the clickedBlock and distance
 * * and decides what to replace at grid[] location clickedBlock
 *
 * * if distance is between 0 and 1
 * * -- replaces with S
 * * if distance is between 2 and 3 or greater than 1 and less and equal to 3
 * * -- replaces with T
 * * if distance is 4 or greater than 3 and less than 5
 * * -- replaces with B
 * * else generates random char with generateRandomChar function and replaces with that
 *
 * @param clickedBlock
 * * int - index of grid[] | clicked block by user.
 * @param distance
 * * double -  distance between clicked block and ghost block.
 */
void fillBlock(int clickedBlock, double distance)
{
    if (distance >= 0 && distance <= 1)
    {
        *(grid + clickedBlock) = 'S';
    }
    else if (distance > 1 && distance <= 3)
    {
        *(grid + clickedBlock) = 'T';
    }
    else if (distance > 3 && distance < 5)
    {
        *(grid + clickedBlock) = 'B';
    }
    else
    {
        *(grid + clickedBlock) = generateRandomChar();
    }
}
/**
 * ? @brief setGhostLocation set the Ghost Location
 * * using generateRandomInteger
 *
 * * gets 2 random numbers and assign
 * * ghostRow and ghostCol with it.
 *
 */
void setGhostLocation()
{
    int randomX = generateRandomInteger(0, rows);
    int randomY = generateRandomInteger(0, cols);

    ghostRow = randomX;
    ghostCol = randomY;
}

// * -------------------- Main Functions

/**
 * ? @brief huntGhost gets the clickedBlock, co-ordinates for ghostBlock and clickedBlock
 * * calculates distance between the co-ordinates and data to fillBlock function
 *
 * ? @param x
 * * int - x pixel for the clicked location
 * ? @param y
 * * int - y pixel for the clicked location
 */
void huntGhost(int x, int y)
{
    // * if ended is false then the game will run.
    if (!ended)
    {
        // * getting block width and height
        int blockWidth = width / cols;
        int blockHeight = height / rows;
        // * getting block Area
        int blockSize = blockWidth * blockHeight;

        // * getting clicked row and clicked column with the help of height and width.
        int clickedRow = y / blockHeight;
        int clickedCol = x / blockWidth;

        // * getting clicked block and ghost block with helper functions
        int clickedBlock = checkCoOrdinates(x, y);
        int ghostBlock = getGhostBlock(ghostRow, ghostCol);

        // * getting distance between clicked block and ghost block
        double distance = distanceCalculator(clickedRow, clickedCol, ghostCol, ghostRow);

        // * calling fillBlock function to finally unlock the lock
        // * and replace it with Bunny, Turtle or Snake
        fillBlock(clickedBlock, distance);
    }
}
/**
 * ? @brief huntGhost gets the co-ordinates clickedBlock
 * * and obtains ghostBlock from getGhostBlock function
 *
 * * if ghostBlock and clickedBlock matches
 * * -- then it replaces clickedBlock index in grid[] with G (Ghost)
 * * -- and the game is won
 *
 * * else
 * * -- replaces clickedBlock index in grid[] with F (Fail)
 * * -- and the games is lost
 *
 *  * also it toggles ended to change the game state to Won/lost.
 *
 * ? @param x
 * * int - x pixel of clicked block
 * ? @param y
 * * int - y pixel of clicked block
 */

void bustGhost(int x, int y)
{
    // * if ended is false then the game will run.
    if (!ended)
    {
        // * gets clicked block and ghost block from the helper functions
        int clickedBlock = checkCoOrdinates(x, y);
        int ghostBlock = getGhostBlock(ghostRow, ghostCol);

        // * checks if clicked block and ghost block are same and decides what to do
        // * G if it is same and F if different
        if (clickedBlock == ghostBlock)
        {
            *(grid + clickedBlock) = 'G';
        }
        else
        {
            *(grid + clickedBlock) = 'F';
        }
        // * toggle ended to end the game
        ended = true;
    }
}

// * ------------------- Initializer Funtion

/**
 * ? @brief initialize function initializes  everything
 * * the grid[] with all the locks
 * * ghost location with helper setGhostLocation
 *
 */

void initialize()
{
    // * creates dynamic grid array with length (rows * cols)
    grid = new char[rows * cols];
    // * adding L (locks) at every location in grid[]
    for (int i = 0; i < rows * cols; i++)
    {
        *(grid + i) = 'L';
    }
    // * setting ghost at random location using setGhostLocation function
    setGhostLocation();
}

/**
// ? @brief quitGhostBuster delete the dynamic array from the memory
 * * and assigns NULL to it.
 * ! important to delete dynamic array and to assign NULL
 * !  So array can be dellocated from the memery and the stored values are also removed
 * * so noone can access them again.
 */
void quitGhostBuster()
{
    delete[] grid;
    grid = NULL;
}