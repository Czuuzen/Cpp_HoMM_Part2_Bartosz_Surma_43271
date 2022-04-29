#include <stdio.h>
#define SDL_MAIN_HANDLED
#include "SDL2/include/SDL.h"
#include "SDL2/include/SDL_image.h"

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
        1920, 1080,
        SDL_WINDOW_SHOWN);

    if (!window)
        return -1;

    // Creating a renderer which will draw things on the screen
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
        return -1;

    // Setting the color of an empty window (RGBA). You are free to adjust it.
    SDL_SetRenderDrawColor(renderer, 20, 150, 39, 255);

    // Loading an image
    char image_path[] = "image.png";
    char board_path[] = "board.png";
    // Here the surface is the information about the image. It contains the color data, width, height and other info.
    SDL_Surface* surface = IMG_Load(image_path);
    if (!surface)
    {
        printf("Unable to load an image %s. Error: %s", image_path, IMG_GetError());
        return -1;
    }
    SDL_Surface* board_surface = IMG_Load(board_path);
    if (!board_surface)
    {
        printf("Unable to load an image %s. Error: %s", image_path, IMG_GetError());
        return -1;
    }

    // Now we use the renderer and the surface to create a texture which we later can draw on the screen.
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        printf("Unable to create a texture. Error: %s", SDL_GetError());
        return -1;
    }
    SDL_Texture* board_texture = SDL_CreateTextureFromSurface(renderer, board_surface);
    if (!board_texture)
    {
        printf("Unable to create a texture. Error: %s", SDL_GetError());
        return -1;
    }

    // In a moment we will get rid of the surface as we no longer /need that. But let's keep the image dimensions.
    int tex_width = surface->w;
    int tex_height = surface->h;

    int board_width = board_surface->w;
    int board_height = board_surface->h;

    // Bye-bye the surface
    SDL_FreeSurface(surface);
    SDL_FreeSurface(board_surface);

    bool done = false;
    SDL_Event sdl_event;

    // The coordinates (could be anything)
    int x = 960;
    int y = 540;

    // Background cords
    int board_x = 960;
    int board_y = 540;

    // Checking at which cell our hero starts
    int s_y_cor = (x / 128);
    int s_x_cor = (y / 98);
    
    // Used to get position of mouse when button is pressed
    int pos_x;
    int pos_y;

    // Used to check in which cell player did click
    int x_cor = -1;
    int y_cor = -1;

    // Used to trigger loops
    int mouse_movement = 0;
    int move = 0;
    bool S = true;
    bool M = true;

    // Used to count ticks
    unsigned int lastTime = 0, currentTime;

    unsigned int board[11][15];

    // Obstacles on battleground
    board[1][1] = board[1][7] = board[1][12] = board[3][9] = board[4][3] = board[5][12] = board[6][6] = board[7][3] = board[7][10] = board[8][13] = board[9][2] = board[9][11] = board[9][9] = 255;
    
    // Free spaces on battleground
    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (board[i][j] != 255) {
                board[i][j] = 0;
            }
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
                    mouse_movement = 1;
                    S = true;
                    M = false;
                }
            }
        }

        // Converting x and y pixels position into suitable cell
        if (mouse_movement != 0) {
           int board_xx = 128;
            int board_yy = 98;
            for (int i = 1; i < 15; i++) {
                if (pos_x > board_xx * (i - 1) && pos_x < board_xx * i) {
                    y_cor = i - 1;
                }
            }
            for (int j = 1; j < 11; j++) {
                if (pos_y > board_yy * (j - 1) && pos_y < board_yy * j) {
                    x_cor = j - 1;
                }
            }
            if (x_cor > 0 && y_cor > 0) {
                mouse_movement = 0;
                move = 1;
            }
            
        }

        // Marking which cell we try moving to
        if (move != 0) {
            if (board[x_cor][y_cor] != 255) {
                board[x_cor][y_cor] = 1;
           }
            move = 0;

        }
        
        // My idea for grassfire?
        while (S) {
            S = false;
            if (board[s_x_cor][s_y_cor] != 0) {
                S = true;
                break;
            }
            for (int j = 1; j < 14; j++) {
                for (int i = 1; i < 10; i++) {
                    if (board[i][j] != 0 && board[i][j] != 255) {
                        board[i][j] += 1;
                        if (i == 1) {

                        }
                        else if (board[i - 1][j] != 255 && board[i - 1][j] < board[i][j]) {
                            board[i - 1][j] = board[i][j] - 1;
                        }
                        if (j == 1) {

                        }
                        else if (board[i][j - 1] != 255 && board[i][j - 1] < board[i][j]) {
                            board[i][j - 1] = board[i][j] - 1;
                        }
                    }
                }
            }
            for (int g = 13; g > 0; g--) {
                for (int k = 9; k > 0; k--) {
                    if (board[k][g] != 255) {
                        if (k == 9) {

                        }
                        else if (board[k + 1][g] != 255 && board[k + 1][g] < board[k][g]) {
                            board[k + 1][g] = board[k][g] - 1;
                        }
                        if (g == 13) {

                        }
                        else if (board[k][g + 1] != 255 && board[k][g + 1] < board[k][g]) {
                            board[k][g + 1] = board[k][g] - 1;
                        }
                    }
                }
            }
        } S = true;


        // Moving our "hero" to destination
        if (currentTime > lastTime + 100) {
            if (board[s_x_cor][s_y_cor] < board[s_x_cor - 1][s_y_cor] && board[s_x_cor - 1][s_y_cor] != 255) {
                y = 98 * (s_x_cor - 1) + 49;
                s_x_cor -= 1;
            }
            else if (board[s_x_cor][s_y_cor] < board[s_x_cor + 1][s_y_cor] && board[s_x_cor + 1][s_y_cor] != 255) {
                y = 98 * (s_x_cor + 1) + 49;
                s_x_cor += 1;
            }
            else if (board[s_x_cor][s_y_cor] < board[s_x_cor][s_y_cor - 1] && board[s_x_cor][s_y_cor - 1] != 255) {
                x = 128 * (s_y_cor - 1) + 64;
                s_y_cor -= 1;
            }
            else if (board[s_x_cor][s_y_cor] < board[s_x_cor][s_y_cor + 1] && board[s_x_cor][s_y_cor + 1] != 255) {
                x = 128 * (s_y_cor + 1) + 64;
                s_y_cor += 1;
            }
            else {
                // Clearing table after alogrithm is complete
                if (board[s_x_cor][s_y_cor] == board[x_cor][y_cor]) {
                    for (int i = 0; i < 11; i++)
                    {
                        for (int j = 0; j < 15; j++)
                        {
                            if (board[i][j] != 255) {
                                board[i][j] = 0;
                            }
                        }
                    }
                    S = false;
                    M = true;
                }
            } lastTime = currentTime;
        }

        // Clearing the screen
        SDL_RenderClear(renderer);

        // All drawing goes here

        // Let's draw a sample image

        
        // Here is the rectangle where the image will be on the screen
        SDL_Rect rect;
        rect.x = (int)round(x - tex_width / 2); // Counting from the image's center but that's up to you
        rect.y = (int)round(y - tex_height / 2); // Counting from the image's center but that's up to you
        rect.w = (int)tex_width;
        rect.h = (int)tex_height;

        SDL_Rect board_rect;
        board_rect.x = (int)round(board_x - board_width / 2); // Counting from the image's center but that's up to you
        board_rect.y = (int)round(board_y - board_height / 2); // Counting from the image's center but that's up to you
        board_rect.w = (int)board_width;
        board_rect.h = (int)board_height;

        SDL_RenderCopyEx(renderer, board_texture, nullptr, &board_rect, 0, nullptr, SDL_FLIP_NONE);
            
        SDL_RenderCopyEx(renderer, // Already know what is that
            texture, // The image
            nullptr, // A rectangle to crop from the original image. Since we need the whole image that can be left empty (nullptr)
            &rect, // The destination rectangle on the screen.
            0, // An angle in degrees for rotation
            nullptr, // The center of the rotation (when nullptr, the rect center is taken)
            SDL_FLIP_NONE); // We don't want to flip the image

// Showing the screen to the player
        SDL_RenderPresent(renderer);

        for (int i = 0; i < 11; i++) {
            for (int j = 0; j < 15; j++) {
                printf("%d ",board[i][j]);
            }printf("\n");
        }
        printf("\n");
        // next frame...
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(board_texture);
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