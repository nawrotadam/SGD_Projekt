#include <SDL2/SDL.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

// check for errors
#define errcheck(e)                         \
    {                                       \
        if (e)                              \
        {                                   \
            cout << SDL_GetError() << endl; \
            SDL_Quit();                     \
            return -1;                      \
        }                                   \
    }

using namespace std;
using namespace std::chrono;

// TODO Wyswietl punkt w miejscu gdzie kliknieto myszka  <- jest
// TODO Prosta animacja punktu sterowanego klawiatura
// TODO Drugi punkt sterowany WSAD-em

int main(int, char **)
{
    const int width = 640;
    const int height = 480;
    int xMouse, yMouse;
    const unsigned int speed = 15;

    errcheck(SDL_Init(SDL_INIT_VIDEO) != 0);

    SDL_Window *window = SDL_CreateWindow(
        "My Next Superawesome Game", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    errcheck(window == nullptr);

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED); // SDL_RENDERER_PRESENTVSYNC
    errcheck(renderer == nullptr);

    // change background color
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    //auto dt = 15ms;
    milliseconds dt(15);

    // draw two players
    SDL_Rect playerRect = {50, 50, 50, 50};
    SDL_Rect playerRect2 = {250, 50, 50, 50};
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x0, 0xFF);
    SDL_RenderFillRect(renderer, &playerRect);
    SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer, &playerRect2);

    steady_clock::time_point current_time = steady_clock::now(); // remember current time
    for (bool game_active = true; game_active;)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        { // check if there are some events
            // quit game
            if (event.type == SDL_QUIT)
            {
                game_active = false;
            }

            // keyboard events
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_UP:
                    cout << "Up" << endl;

                    playerRect.y = playerRect.y - speed;
                    SDL_RenderFillRect(renderer, &playerRect);

                    break;
                case SDLK_DOWN:
                    cout << "Down" << endl;
                    break;
                case SDLK_LEFT:
                    cout << "Left" << endl;
                    break;
                case SDLK_RIGHT:
                    cout << "Right" << endl;
                    break;
                }

                // draw();
            }

            // mouse events
            // if (event.type == SDL_MOUSEMOTION)
            // { 
            //     SDL_GetGlobalMouseState(&xMouse, &yMouse);  // save mouse position to variables
            // }
            if (event.type == SDL_MOUSEMOTION && event.button.button == SDL_BUTTON(SDL_BUTTON_LEFT))
            {
                cout << "Left mouse button" << endl;

                SDL_GetMouseState(&xMouse, &yMouse);
                SDL_SetRenderDrawColor(renderer, 0xFF, 0x0, 0x0, 0xFF);
                SDL_RenderDrawPoint(renderer, xMouse, yMouse);
            }
            if (event.type == SDL_MOUSEMOTION && event.button.button == SDL_BUTTON(SDL_BUTTON_RIGHT))
            {
                cout << "Right mouse button" << endl;
            }
        }

        SDL_RenderPresent(renderer); // draw frame to screen

        this_thread::sleep_until(current_time = current_time + dt);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
