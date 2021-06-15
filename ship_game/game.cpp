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
#include <stdlib.h>

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
const char* ISLAND_PATH = "ship_game/textures/island.png";
const char* DEBUG_TEXTURE_PATH = "ship_game/textures/debug_texture.png";
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

bool is_ship_destroyed(int hull)
{
    if (hull <= 0)
    {
        cout<<"Your ship has sinked"<<endl;
        return true;
    } 
    else return false;
}

int main(int, char **)
{
    int xMouse, yMouse;
    int ship_speed = 7;
    int hull = 100;
    int damage = 1;
    bool in_harbor = false;
    
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

    // initialize debug texture
    SDL_Surface* temp_debug_surface = IMG_Load(DEBUG_TEXTURE_PATH);
    SDL_Texture* debug_texture = SDL_CreateTextureFromSurface(renderer, temp_debug_surface);
    SDL_FreeSurface(temp_debug_surface);

    // initialize sea texture
    SDL_Surface* temp_sea_surface = IMG_Load(SEA_PATH);
    SDL_Texture* sea_texture = SDL_CreateTextureFromSurface(renderer, temp_sea_surface);
    SDL_FreeSurface(temp_sea_surface);

    // initialize ship texture
    SDL_Surface* temp_ship_surface = IMG_Load(SHIP_PATH);
    SDL_Texture* ship_texture = SDL_CreateTextureFromSurface(renderer, temp_ship_surface);
    SDL_FreeSurface(temp_ship_surface);

    // initialize island texture
    SDL_Surface* temp_island_surface = IMG_Load(ISLAND_PATH);
    SDL_Texture* island_texture = SDL_CreateTextureFromSurface(renderer, temp_island_surface);
    SDL_FreeSurface(temp_island_surface);

    // declare hull text position
    SDL_Rect text_position;
    text_position.x = 20;
    text_position.y = 10;
    text_position.w = 55;
    text_position.h = 25;

    // declare initial ship position
    SDL_Rect ship_position;
    ship_position.x = width/2;
    ship_position.y = height/2;
    ship_position.w = 24;
    ship_position.h = 24;

    // declare initial ship collision square possition
    SDL_Rect ship_collision;
    ship_collision.x = ship_position.x;
    ship_collision.y = ship_position.y;
    ship_collision.w = ship_position.w;
    ship_collision.h = ship_position.h;

    // declare initial whole island position
    SDL_Rect island_position;
    island_position.x = width-250;
    island_position.y = 0;
    island_position.w = 250;
    island_position.h = 460;

    // declare initial island first chunk position
    SDL_Rect island_chunk_one_position;
    island_chunk_one_position.w = 250;
    island_chunk_one_position.h = 92;
    island_chunk_one_position.x = width - island_chunk_one_position.w;
    island_chunk_one_position.y = 0;

    // declare initial island second chunk position
    SDL_Rect island_chunk_two_position;
    island_chunk_two_position.w = 122;
    island_chunk_two_position.h = 78;
    island_chunk_two_position.x = width - island_chunk_two_position.w;
    island_chunk_two_position.y = 92;

    // declare initial island third chunk position
    SDL_Rect island_chunk_three_position;
    island_chunk_three_position.w = 190;
    island_chunk_three_position.h = 89;
    island_chunk_three_position.x = width - island_chunk_three_position.w;
    island_chunk_three_position.y = 170;

    // declare initial island fourth chunk position
    SDL_Rect island_chunk_four_position;
    island_chunk_four_position.w = 122;
    island_chunk_four_position.h = 57;
    island_chunk_four_position.x = width - island_chunk_four_position.w;
    island_chunk_four_position.y = 259;

    // declare initial island fifth chunk position
    SDL_Rect island_chunk_five_position;
    island_chunk_five_position.w = 82;
    island_chunk_five_position.h = 77;
    island_chunk_five_position.x = width-island_chunk_five_position.w;
    island_chunk_five_position.y = 316;

    // declare initial island sixth chunk position
    SDL_Rect island_chunk_six_position;
    island_chunk_six_position.w = 167;
    island_chunk_six_position.h = 67;
    island_chunk_six_position.x = width-island_chunk_six_position.w;
    island_chunk_six_position.y = 393;

    // declare initial harbor position
    SDL_Rect harbor_position;
    harbor_position.w = 41;
    harbor_position.h = 78;
    harbor_position.x = island_chunk_two_position.x - harbor_position.w;
    harbor_position.y = island_chunk_two_position.y;

    // draw initial frame of sea texture
    SDL_RenderCopy(renderer, sea_texture, NULL, NULL);

    // draw initial frame of island texture
    SDL_RenderCopy(renderer, island_texture, NULL, &island_position);

    // draw initial frame of hull texture
    render_hull_text(*renderer, *roboto_bold_font, text_position, "Hull: " + to_string(hull));

    // draw initial frame of ship texture
    SDL_RenderCopy(renderer, ship_texture, NULL, &ship_position);

    // draw initial frame of ship debug texture
    SDL_RenderCopy(renderer, debug_texture, NULL, &ship_collision);

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
                    ship_position.y -= ship_speed;
                    ship_collision.y -= ship_speed;
                    break;
                case SDLK_DOWN:
                    ship_position.y += ship_speed;
                    ship_collision.y += ship_speed;
                    break;
                case SDLK_LEFT:
                    ship_position.x -= ship_speed;
                    ship_collision.x -= ship_speed;
                    break;
                case SDLK_RIGHT:
                    ship_position.x += ship_speed;
                    ship_collision.x += ship_speed;
                    break;
                case SDLK_w:
                    ;
                    break;
                case SDLK_s:
                    ;
                    break;
                case SDLK_a:
                    ;
                    break;
                case SDLK_d:
                    ;
                    break;
                case SDLK_SPACE:
                    hull -= 10;  // TODO debug only, change it later
                    break;
                }
            }

            // mouse events
            if (event.type == SDL_MOUSEMOTION && event.button.button == SDL_BUTTON(SDL_BUTTON_LEFT))
            {
                ;
            }
            if (event.type == SDL_MOUSEMOTION && event.button.button == SDL_BUTTON(SDL_BUTTON_RIGHT))
            {
                ;
            }
        }

        // check whether ship is destroyed
        if(is_ship_destroyed(hull))
            break;

        // draw textures
        SDL_RenderCopy(renderer, sea_texture, NULL, NULL);  // sea
        SDL_RenderCopy(renderer, island_texture, NULL, &island_position);  // island
        // SDL_RenderCopy(renderer, debug_texture, NULL, &ship_collision);  // ship collision debug
        SDL_RenderCopy(renderer, ship_texture, NULL, &ship_position);  // ship
        render_hull_text(*renderer, *roboto_bold_font, text_position, "Hull: " + to_string(hull));  // hull text

        // debug
        SDL_RenderCopy(renderer, debug_texture, NULL, &harbor_position);

        // collision with islands
        if(SDL_HasIntersection(&ship_collision, &island_chunk_one_position) || 
        SDL_HasIntersection(&ship_collision, &island_chunk_two_position) ||
        SDL_HasIntersection(&ship_collision, &island_chunk_three_position) ||
        SDL_HasIntersection(&ship_collision, &island_chunk_four_position) ||
        SDL_HasIntersection(&ship_collision, &island_chunk_five_position) ||
        SDL_HasIntersection(&ship_collision, &island_chunk_six_position))
        {
            cout<<"Island hitted"<<endl;
            hull -= damage;
        }

        // collision with harbor
        if(SDL_HasIntersection(&ship_collision, &harbor_position))
        {
            cout<<"Harbor hitted"<<endl;
            in_harbor = true;
        }
        
        if(in_harbor)
        {
            cout<<"You have come to the Campeche city"<<endl;
            cout<<endl;
            cout<<"Click t to talk to city mayor"<<endl;
            cout<<"Click r to repair hull"<<endl;
            cout<<"Click l to leave city"<<endl;

            while(in_harbor)
            {
                SDL_Event event;
                while (SDL_PollEvent(&event))
                {
                    // quit game
                    if (event.type == SDL_QUIT)
                    {
                        exit(EXIT_FAILURE);
                    }

                    if (event.type == SDL_KEYDOWN)
                    {
                        switch (event.key.keysym.sym)
                        {
                        case SDLK_t:
                            cout<<"Hello stranger! I'm sure i haven't seen you before. Got some time to help?"<<endl;
                            cout<<"I have been struggling with one problem and you look like the one that can manage this."<<endl;
                            cout<<"Pirates are becoming bigger and bigger problem with time passing, they don't bother us directly but they attack merchants ships."<<endl;
                            cout<<"If it continues we will lost all trade treaties and the city ecconomy will collapse."<<endl;
                            cout<<"You say you are intrested? Good. Let me show where these scumbags hide..."<<endl;
                            cout<<endl;
                            cout<<"You got your first mission!"<<endl;
                            break;
                        case SDLK_r:
                            hull = 100;
                            cout<<"Hull repaired!"<<endl;
                            break;
                        case SDLK_l:
                            ship_position.x -= 50;
                            ship_collision.x -= 50;
                            in_harbor = false;
                            break;
                        }
                    }
                }
            }
        }

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
