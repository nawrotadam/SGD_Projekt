#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL_ttf.h>
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
const SDL_Color font_color = {255, 255, 255};

const char* SEA_PATH = "ship_game/textures/sea.png";
const char* SHIP_PATH = "ship_game/textures/ship.png";
const char* ROBOTO_BOLD_PATH = "ship_game/fonts/Roboto-Bold.ttf";
const char* ROBOTO_REGULAR_PATH = "ship_game/fonts/Roboto-Regular.ttf";
const char* ROBOTO_THIN_PATH = "ship_game/fonts/Roboto-Thin.ttf";

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

void render_hull_text(SDL_Renderer &renderer, TTF_Font &used_font, SDL_Rect &text_position, string text)
{
    SDL_Surface * hull_surface = TTF_RenderText_Solid(&used_font, text.c_str(), font_color);
    SDL_Texture * hull_texture = SDL_CreateTextureFromSurface(&renderer, hull_surface);
    SDL_FreeSurface(hull_surface);

    SDL_RenderCopy(&renderer, hull_texture, NULL, &text_position);  // draw texture on screen
    SDL_DestroyTexture(hull_texture);
}

bool check_hull_lvl(int hull)
{
    if (hull < 0)
    {
        cout<<"Your ship has sinked"<<endl;
        return false;
    } 
    else return true;
}

int main(int, char **)
{
    int xMouse, yMouse;
    int ship_speed = 3;
    int hull = 100;
    
    errcheck(SDL_Init(SDL_INIT_VIDEO) != 0);
    errcheck(TTF_Init() != 0);

    SDL_Window *window = SDL_CreateWindow(
        "Ship game", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    errcheck(window == nullptr);

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED); // SDL_RENDERER_PRESENTVSYNC
    errcheck(renderer == nullptr);

    // initialize fonts
    TTF_Font * roboto_bold_font = TTF_OpenFont(ROBOTO_BOLD_PATH, 25);

    // initialize sea texture
    SDL_Surface* temp_sea_surface = IMG_Load(SEA_PATH);
    SDL_Texture* sea_texture = SDL_CreateTextureFromSurface(renderer, temp_sea_surface);
    SDL_FreeSurface(temp_sea_surface);

    // initialize ship texture
    SDL_Surface* temp_ship_surface = IMG_Load(SHIP_PATH);
    SDL_Texture* ship_texture = SDL_CreateTextureFromSurface(renderer, temp_ship_surface);
    SDL_FreeSurface(temp_ship_surface);

    // declare hull text position
    SDL_Rect text_position;
    text_position.x = 20;
    text_position.y = 10;
    text_position.w = 55;
    text_position.h = 25;

    // draw initial frame of sea texture
    SDL_RenderCopy(renderer, sea_texture, NULL, NULL);

    // draw initial frame of hull texture
    render_hull_text(*renderer, *roboto_bold_font, text_position, "Hull: " + to_string(hull));

    // declare initial ship position
    SDL_Rect ship_position;
    ship_position.x = width/2;
    ship_position.y = height/2;
    ship_position.w = 32;
    ship_position.h = 32;

    // draw initial frame of ship texture
    SDL_RenderCopy(renderer, ship_texture, NULL, &ship_position);

    //auto dt = 15ms;
    milliseconds dt(15);

    SDL_RenderPresent(renderer); // draw frame to screen
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
                    ship_position.y -= ship_speed;
                    break;
                case SDLK_DOWN:
                    cout << "Down" << endl;
                    ship_position.y += ship_speed;
                    break;
                case SDLK_LEFT:
                    cout << "Left" << endl;
                    ship_position.x -= ship_speed;
                    break;
                case SDLK_RIGHT:
                    cout << "Right" << endl;
                    ship_position.x += ship_speed;
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
                    hull -= 10;  // TODO debug only, change it later
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
        SDL_RenderCopy(renderer, sea_texture, NULL, NULL);  // sea
        SDL_RenderCopy(renderer, ship_texture, NULL, &ship_position);  // ship
        render_hull_text(*renderer, *roboto_bold_font, text_position, "Hull: " + to_string(hull));  // hull text

        SDL_RenderPresent(renderer); // draw frame to screen
        this_thread::sleep_until(current_time = current_time + dt);
    }
    TTF_CloseFont(roboto_bold_font);
    SDL_DestroyTexture(sea_texture);
    SDL_DestroyTexture(ship_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
