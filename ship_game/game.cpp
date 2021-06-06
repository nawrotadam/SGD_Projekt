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

using namespace std;
using namespace std::chrono;

// check for errors
#define errcheck(e)                                                            \
  {                                                                            \
    if (e) {                                                                   \
      cout << SDL_GetError() << endl;                                          \
      SDL_Quit();                                                              \
      return -1;                                                               \
    }                                                                          \
  }

const int width = 640;
const int height = 480;

SDL_Point rotate_point(double cx, double cy, double angle, SDL_Point p)
{
    double pi = acos(-1);
    double rotation_angle = (double)angle / 180.0 * pi;
    double s = sin(rotation_angle);
    double c = cos(rotation_angle);

    // translate point back to origin:
    p.x -= cx;
    p.y -= cy;

    // rotate point
    double xnew = p.x * c - p.y * s;
    double ynew = p.x * s + p.y * c;

    // translate point back:
    p.x = xnew + cx;
    p.y = ynew + cy;

    return p;
}

int main(int, char **)
{
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
        while (SDL_PollEvent(&event))  // check if there are some events
        {
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
                case SDLK_w:
                    cout << "W" << endl;
                    break;
                case SDLK_s:
                    cout << "S" << endl;
                    break;
                case SDLK_a:
                    cout << "A" << endl;
                    break;
                case SDLK_d:
                    cout << "D" << endl;
                    break;
                case SDLK_SPACE:
                    cout << "Space" << endl;
                    break;
                }

                // clear map
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);
            }

            // mouse events
            if (event.type == SDL_MOUSEMOTION && event.button.button == SDL_BUTTON(SDL_BUTTON_LEFT))
            {
                cout << "Left mouse button" << endl;
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
