#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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

const int width = 680;
const int height = 460;
const char* SEA_PATH = "ship_game/textures/sea.png";
const char* SHIP_PATH = "ship_game/textures/ship.png";

class Ship {
private:
    SDL_Point position;
    float dx, dy;  // velocity
public:
    Ship(SDL_Point position, double dx, double dy) {
        this->position = position;
        this->dx = dx;
        this->dy = dy;
    }

    SDL_Point getPosition() {
        return position;
    }

    void setPosition(SDL_Point position) {
        this->position = position;
    }

    double getVelocityX() {
        return dx;
    }

    double setVelocityX(double dx) {
        this->dx = dx;
    }

    double getVelocityY() {
        return dy;
    }

    double setVelocityY(double dy) {
        this->dy = dy;
    }
};

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
        "Ship game", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    errcheck(window == nullptr);

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED); // SDL_RENDERER_PRESENTVSYNC
    errcheck(renderer == nullptr);

    // initialize sea texture
    SDL_Surface* temp_sea_surface = IMG_Load(SEA_PATH);
    SDL_Texture* sea_texture = SDL_CreateTextureFromSurface(renderer, temp_sea_surface);
    SDL_FreeSurface(temp_sea_surface);

    // initialize ship texture
    SDL_Surface* temp_ship_surface = IMG_Load(SHIP_PATH);
    SDL_Texture* ship_texture = SDL_CreateTextureFromSurface(renderer, temp_ship_surface);
    SDL_FreeSurface(temp_ship_surface);

    // draw initial frame of sea texture
    SDL_RenderCopy(renderer, sea_texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // declare initial ship position
    SDL_Rect ship_position;
    ship_position.x = width/2;
    ship_position.y = height/2;
    ship_position.w = 32;
    ship_position.h = 32;

    // draw initial frame of ship texture
    SDL_RenderCopy(renderer, ship_texture, NULL, &ship_position);
    SDL_RenderPresent(renderer);

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

        // draw textures
        SDL_RenderCopy(renderer, sea_texture, NULL, NULL);  // sea texture
        SDL_RenderCopy(renderer, ship_texture, NULL, &ship_position);  // ship texture

        SDL_RenderPresent(renderer); // draw frame to screen
        this_thread::sleep_until(current_time = current_time + dt);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
