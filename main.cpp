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

// TODO Wyswietl punkt w miejscu gdzie kliknieto myszka
// TODO Prosta animacja punktu sterowanego klawiatura
// TODO Drugi punkt sterowany WSAD-em

void mousePress(SDL_MouseButtonEvent &b)
{
    if (b.button == SDL_BUTTON_LEFT)
    {
        cout << "Left click" << endl;
    }
}

int main(int, char **)
{
    using namespace std;
    using namespace std::chrono;
    const int width = 640;
    const int height = 480;
    int xMouse, yMouse;

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
            }

            // mouse events
            if (event.type == SDL_MOUSEMOTION)
            { 
                SDL_GetGlobalMouseState(&xMouse, &yMouse);  // save mouse position to variables
            }
            if (event.type == SDL_MOUSEMOTION && event.button.button == SDL_BUTTON(SDL_BUTTON_LEFT))
            {
                cout << "Left mouse button" << endl;

                SDL_SetRenderDrawColor(renderer, 0xFF, 0x0, 0x0, 0xFF);
                SDL_RenderDrawPoint(renderer, xMouse, yMouse);
                
                cout<<"xMouse: "<<xMouse<<endl;
                cout<<"yMouse: "<<yMouse<<endl;
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
