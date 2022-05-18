#include <stdio.h>
#define SDL_MAIN_HANDLED
#include "SDL2/include/SDL.h"
#include "SDL2/include/SDL_image.h"

const int TABLE_WIDTH = 15;
const int TABLE_HEIGHT = 11;
const int CELL_WIDTH = 128;
const int CELL_HEIGHT = 98;
const int OBSTACLE = 255;
const int DELAY = 100;
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

// The coordinates (could be anything)
float x = 960;
float y = 540;

// Background cords
float board_x = 960;
float board_y = 540;

// Checking at which cell our hero starts
int s_y_cor = (x / CELL_WIDTH);
int s_x_cor = (y / CELL_HEIGHT);

// Used to get position of mouse when button is pressed
int pos_x;
int pos_y;

// Used to check in which cell player did click
int x_cor = -1;
int y_cor = -1;

// Used to trigger loops
bool mouse_movement = true;
bool move = true;
bool S = true;
bool M = true;

// Used to count ticks
unsigned int lastTime = 0, currentTime;

unsigned int board[TABLE_HEIGHT][TABLE_WIDTH];

struct Vec2i
{
   int x;
   int y;
};

struct Vec2ixy
{
    int z;
    int w;
    int x = (z / CELL_WIDTH);
    int y = (w / CELL_HEIGHT);
};

struct Vec4i {
    int x;
    int y;
    int z;
    int w;
};

struct Vec2f
{
    float x;
    float y;
};

struct Image {
    void Init(const char* filename, SDL_Renderer* renderer);
    void Init(Vec4i color, Vec2i size, SDL_Renderer* renderer);
    void Render(SDL_Renderer* renderer, Vec2f position);
    void Destroy();

    SDL_Texture* texture;
    Vec2i texSize;
};

void Image::Init(const char* filename, SDL_Renderer* renderer) 
{
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface)
    {
        printf("Unable to load an image %s. Error: %s", filename, IMG_GetError());
        return;
    }
    

    // Now we use the renderer and the surface to create a texture which we later can draw on the screen.
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        printf("Unable to create a texture. Error: %s", SDL_GetError());
        return;
    }
    

    // In a moment we will get rid of the surface as we no longer /need that. But let's keep the image dimensions.

    texSize = { surface->w, surface->h };

    SDL_FreeSurface(surface);
}

void Image::Init(Vec4i color, Vec2i size, SDL_Renderer* renderer)
{
    SDL_Surface* s = SDL_CreateRGBSurface(0, size.x, size.y, 32, color.x, color.y, color.z, color.w);
    texture = SDL_CreateTextureFromSurface(renderer, s), size.x, size.y;
    SDL_FreeSurface(s);
    texSize = size;

}

void Image::Destroy() {
    SDL_DestroyTexture(texture);
}

void Image::Render(SDL_Renderer* renderer, Vec2f position)
{
    SDL_Rect rect;
    rect.x = (int)round(position.x - texSize.x/2); // Counting from the image's center but that's up to you
    rect.y = (int)round(position.y - texSize.y/2); // Counting from the image's center but that's up to you
    rect.w = (int)texSize.x;
    rect.h = (int)texSize.y;

    SDL_RenderCopyEx(renderer, // Already know what is that
        texture, // The image
        nullptr, // A rectangle to crop from the original image. Since we need the whole image that can be left empty (nullptr)
        &rect, // The destination rectangle on the screen.
        0, // An angle in degrees for rotation
        nullptr, // The center of the rotation (when nullptr, the rect center is taken)
        SDL_FLIP_NONE); // We don't want to flip the image
}

struct Character
{
    void Init(const char* filename, SDL_Renderer* renderer);
    void Destroy();

    void Render(SDL_Renderer* renderer);
    void Movement(Vec2ixy cell_position);

    Image image;
    Vec2f position;
    Vec2ixy cell_position;


};

void Character::Init(const char* filename, SDL_Renderer* renderer)
{
    image.Init(filename, renderer);
   
}

void Character::Destroy()
{
    image.Destroy();
}

void Character::Render(SDL_Renderer* renderer)
{
    image.Render(renderer, position);
}
Character char1;
void Movementu(int s_x_cor, int s_y_cor, float y, float x) {
    if (currentTime > lastTime + DELAY) {
        if (board[s_x_cor][s_y_cor] < board[s_x_cor - 1][s_y_cor] && board[s_x_cor - 1][s_y_cor] != OBSTACLE) {
            y = CELL_HEIGHT * (s_x_cor - 1) + CELL_HEIGHT / 2;
            s_x_cor -= 1;
            char1.cell_position.x = s_x_cor;
            char1.position.y = y;
        }
        else if (board[s_x_cor][s_y_cor] < board[s_x_cor + 1][s_y_cor] && board[s_x_cor + 1][s_y_cor] != OBSTACLE) {
            y = CELL_HEIGHT * (s_x_cor + 1) + CELL_HEIGHT / 2;
            s_x_cor += 1;
            char1.cell_position.x = s_x_cor;
            char1.position.y = y;
        }
        else if (board[s_x_cor][s_y_cor] < board[s_x_cor][s_y_cor - 1] && board[s_x_cor][s_y_cor - 1] != OBSTACLE) {
            x = CELL_WIDTH * (s_y_cor - 1) + CELL_WIDTH / 2;
            s_y_cor -= 1;
            char1.cell_position.y = s_x_cor;
            char1.position.x = x;
        }
        else if (board[s_x_cor][s_y_cor] < board[s_x_cor][s_y_cor + 1] && board[s_x_cor][s_y_cor + 1] != OBSTACLE) {
            x = CELL_WIDTH * (s_y_cor + 1) + CELL_WIDTH / 2;
            s_y_cor += 1;
            char1.cell_position.y = s_x_cor;
            char1.position.x = x;
        }
        else {
            // Clearing table after alogrithm is complete
            if (board[s_x_cor][s_y_cor] == board[x_cor][y_cor]) {
                for (int i = 0; i < TABLE_HEIGHT; i++)
                {
                    for (int j = 0; j < TABLE_WIDTH; j++)
                    {
                        if (board[i][j] != OBSTACLE) {
                            board[i][j] = 0;
                        }
                    }
                }
                S = false;
                M = true;
            }
        } lastTime = currentTime;
    }
}

void Character::Movement(Vec2ixy cell_position)
{
    Movementu(cell_position.x, cell_position.y, cell_position.w, cell_position.z);
}





int main()
{
    
    // Init SDL libraries
    SDL_SetMainReady(); // Just leave it be
    int result = 0;
    result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO); // Init of the main SDL library
    if (result) // SDL_Init returns 0 (false) when everything is OK
    {
        printf("Can't initialize SDL. Error: %s", SDL_GetError()); // SDL_GetError() returns a string (as const char*) which explains what went wrong with the last operation
        return -1;
    }

    result = IMG_Init(IMG_INIT_PNG); // Init of the Image SDL library. We only need to support PNG for this project
    if (!(result & IMG_INIT_PNG)) // Checking if the PNG decoder has started successfully
    {
        printf("Can't initialize SDL image. Error: %s", SDL_GetError());
        return -1;
    }

    // Creating the window 1920x1080 (could be any other size)
    SDL_Window* window = SDL_CreateWindow("FirstSDL",
        0, 0,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN);

    if (!window)
        return -1;

    // Creating a renderer which will draw things on the screen
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
        return -1;
    //Image BlackImage;
    //Image black_image;
    //black_image.Init({ 0, 0, 0, 255 }, { CELL_WIDTH, CELL_HEIGHT }, renderer);
    // Setting the color of an empty window (RGBA). You are free to adjust it.
    SDL_SetRenderDrawColor(renderer, 220, 20, 39, 255);

    // Loading an image
    char board_path[] = "board.png";
    // Here the surface is the information about the image. It contains the color data, width, height and other info.
    //Image ig;

    char1.cell_position = {640, 400};
    char1.position = { (float)char1.cell_position.z, (float)char1.cell_position.w };
    char1.Init("image.png", renderer);
    //Image bg;
    //bg.Init("board.png", renderer);

    bool done = false;
    SDL_Event sdl_event;

    

    // Obstacles on battleground
    board[1][1] = board[1][7] = board[1][12] = board[3][9] = board[4][3] = board[5][12] = board[6][6] = board[7][3] = board[7][10] = board[8][13] = board[9][2] = board[9][11] = board[9][9] = OBSTACLE;
    // Free spaces on battleground
    for (int i = 0; i < TABLE_HEIGHT; i++)
    {
        for (int j = 0; j < TABLE_WIDTH; j++)
        {
            if (board[i][j] != OBSTACLE) {
                board[i][j] = 0;
            }
            /*else if (board[i][j] == OBSTACLE) {
                black_image.Render(renderer, { (float)j, (float)i });
                
            }*/
        }
    }
    // The main loop
    // Every iteration is a frame
    while (!done)
    {
        currentTime = SDL_GetTicks();
        // Polling the messages from the OS.
        // That could be key downs, mouse movement, ALT+F4 or many others
        while (SDL_PollEvent(&sdl_event) && M)
        {
            if (sdl_event.type == SDL_QUIT) // The user wants to quit
            {
                done = true;
            }
            else if (sdl_event.type == SDL_KEYDOWN) // A key was pressed
            {
                switch (sdl_event.key.keysym.sym) // Which key?
                {
                case SDLK_ESCAPE: // Posting a quit message to the OS queue so it gets processed on the next step and closes the game
                    SDL_Event event;
                    event.type = SDL_QUIT;
                    event.quit.type = SDL_QUIT;
                    event.quit.timestamp = SDL_GetTicks();
                    SDL_PushEvent(&event);
                    break;
                    // Other keys here
                default:
                    break;
                }
            }
            if (sdl_event.type == SDL_MOUSEBUTTONDOWN) {
                if (sdl_event.button.button == SDL_BUTTON_LEFT || sdl_event.button.button == SDL_BUTTON_RIGHT)
                {
                    SDL_GetMouseState(&pos_x, &pos_y);
                    mouse_movement = false;
                    S = true;
                    M = false;
                }
            }
        }

        // Converting x and y pixels position into suitable cell
        if (!mouse_movement) {
            for (int i = 1; i < TABLE_WIDTH; i++) {
                if (pos_x > CELL_WIDTH * (i - 1) && pos_x < CELL_WIDTH * i) {
                    y_cor = i - 1;
                }
            }
            for (int j = 1; j < TABLE_HEIGHT; j++) {
                if (pos_y > CELL_HEIGHT * (j - 1) && pos_y < CELL_HEIGHT * j) {
                    x_cor = j - 1;
                }
            }
            if (x_cor > 0 && y_cor > 0) {
                mouse_movement = true;
                move = false;
            }

        }

        // Marking which cell we try moving to
        if (!move) {
            if (board[x_cor][y_cor] != OBSTACLE) {
                board[x_cor][y_cor] = 1;
            }
            move = true;

        }

        // My idea for grassfire?
        while (S) {
            S = false;
            if (board[char1.cell_position.x][char1.cell_position.y] != 0) {
                S = true;
                break;
            }
            for (int j = 1; j < TABLE_WIDTH - 1; j++) {
                for (int i = 1; i < TABLE_HEIGHT - 1; i++) {
                    if (board[i][j] != 0 && board[i][j] != OBSTACLE) {
                        board[i][j] += 1;
                        if (i == 1) {

                        }
                        else if (board[i - 1][j] != OBSTACLE && board[i - 1][j] < board[i][j]) {
                            board[i - 1][j] = board[i][j] - 1;
                        }
                        if (j == 1) {

                        }
                        else if (board[i][j - 1] != OBSTACLE && board[i][j - 1] < board[i][j]) {
                            board[i][j - 1] = board[i][j] - 1;
                        }
                    }
                }
            }
            for (int g = TABLE_WIDTH - 2; g > 0; g--) {
                for (int k = TABLE_HEIGHT - 2; k > 0; k--) {
                    if (board[k][g] != OBSTACLE) {
                        if (k == 9) {

                        }
                        else if (board[k + 1][g] != OBSTACLE && board[k + 1][g] < board[k][g]) {
                            board[k + 1][g] = board[k][g] - 1;
                        }
                        if (g == 13) {

                        }
                        else if (board[k][g + 1] != OBSTACLE && board[k][g + 1] < board[k][g]) {
                            board[k][g + 1] = board[k][g] - 1;
                        }
                    }
                }
            }
        } 
        S = true;


        // Moving our "hero" to destination
        char1.Movement({ char1.cell_position.x, char1.cell_position.y, char1.cell_position.w, char1.cell_position.z });
        // Clearing the screen
        SDL_RenderClear(renderer);

        // All drawing goes here

        // Let's draw a sample image

        
        // Here is the rectangle where the image will be on the screen
        

        
        //bg.Render(renderer, {board_x, board_y});
        
        char1.Render(renderer);
            
// Showing the screen to the player
        SDL_RenderPresent(renderer);

        // next frame...
    }

    char1.Destroy();
   // bg.Destroy();
    // If we reached here then the main loop stoped
    // That means the game wants to quit

    // Shutting down the renderer
    SDL_DestroyRenderer(renderer);

    // Shutting down the window
    SDL_DestroyWindow(window);

    // Quitting the Image SDL library
    IMG_Quit();
    // Quitting the main SDL library
    SDL_Quit();

    // Done.
    return 0;
}