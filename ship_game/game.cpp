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

#include "game.hpp"

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
const char* ISLAND_PATH = "ship_game/textures/island.png";
const char* DEBUG_TEXTURE_PATH = "ship_game/textures/debug_texture.png";
const char* ROBOTO_BOLD_PATH = "ship_game/fonts/Roboto-Bold.ttf";
const char* ROBOTO_REGULAR_PATH = "ship_game/fonts/Roboto-Regular.ttf";
const char* ROBOTO_THIN_PATH = "ship_game/fonts/Roboto-Thin.ttf";


int main(int, char **)
{
    int xMouse, yMouse;
    double ship_speed_x = 7;
    double ship_speed_y = 7;
    int hull = 100;
    int damage = 1;
    bool in_harbor = false;
    vector<Bullet> bullets;
    int bullet_size_w = 10;
    int bullet_size_h = 10;
    
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
    TTF_Font* roboto_bold_font = TTF_OpenFont(ROBOTO_BOLD_PATH, 25);

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
    SDL_Rect text_position = {20, 10, 55, 25};

    // declare initial whole island position - used to draw its texture
    SDL_Rect island_position = {width-250, 0, 250, 460};

    // declare island chunks position - used for collision
    SDL_Rect island_chunk_one_position = {width - island_chunk_one_position.w, 0, 250, 92};
    SDL_Rect island_chunk_two_position = {width - island_chunk_two_position.w, 92, 122, 78};
    SDL_Rect island_chunk_three_position = {width - island_chunk_three_position.w, 170, 190, 89};
    SDL_Rect island_chunk_four_position = {width - island_chunk_four_position.w, 259, 122, 57};
    SDL_Rect island_chunk_five_position = {width-island_chunk_five_position.w, 316, 82, 77};
    SDL_Rect island_chunk_six_position = {width-island_chunk_six_position.w, 393, 167, 67};

    // declare initial harbor position
    SDL_Rect harbor_position = {island_chunk_two_position.x - harbor_position.w, 92, 41, 78};

    // declare initial ship position
    SDL_Rect ship_position = {width/2, height/2, 24, 24};

    // initialize ship object
    Ship ship(ship_position, ship_speed_x, ship_speed_y);

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
                    ship.move(0, -ship_speed_y);      
                    break;
                case SDLK_DOWN:
                    ship.move(0, +ship_speed_y);   
                    break;
                case SDLK_LEFT:
                    ship.move(-ship_speed_x, 0);   
                    break;
                case SDLK_RIGHT:
                    ship.move(+ship_speed_x, 0);   
                    break;
                case SDLK_w:
                    ship.move(0, -ship_speed_y);  
                    break;
                case SDLK_s:
                    ship.move(0, +ship_speed_y); 
                    break;
                case SDLK_a:
                    ship.move(-ship_speed_x, 0);   
                    break;
                case SDLK_d:
                    ship.move(+ship_speed_x, 0); 
                    break;
                case SDLK_SPACE:
                    SDL_Rect bullet_pos = {ship_position.x, ship_position.y, bullet_size_w, bullet_size_h};
                    float bullet_speed_x = -10;
                    float bullet_speed_y = 3;
                    Bullet new_bullet(bullet_pos, bullet_speed_x, bullet_speed_y);
                    bullets.push_back(new_bullet);
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

        // pos stored in variable to avoid multiple calls to position getter
        auto ship_pos = ship.get_position();

        // collision with islands
        if(SDL_HasIntersection(&ship_pos, &island_chunk_one_position) || 
        SDL_HasIntersection(&ship_pos, &island_chunk_two_position) ||
        SDL_HasIntersection(&ship_pos, &island_chunk_three_position) ||
        SDL_HasIntersection(&ship_pos, &island_chunk_four_position) ||
        SDL_HasIntersection(&ship_pos, &island_chunk_five_position) ||
        SDL_HasIntersection(&ship_pos, &island_chunk_six_position))
        {
            cout<<"Island hitted"<<endl;
            hull -= damage;
        }

        // collision with harbor
        if(SDL_HasIntersection(&ship_pos, &harbor_position))
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
                            ship.move(-ship_speed_x, 0);   
                            in_harbor = false;
                            break;
                        }
                    }
                }
            }
        }

        // draw textures
        SDL_RenderCopy(renderer, sea_texture, NULL, NULL);  // sea
        SDL_RenderCopy(renderer, island_texture, NULL, &island_position);  // island
        SDL_RenderCopy(renderer, ship_texture, NULL, &ship_pos);  // ship
        render_hull_text(*renderer, *roboto_bold_font, text_position, "Hull: " + to_string(hull));  // hull text

        // update objects
        for(auto& el: bullets)
            el.Update();

        // draw bullets
        for(auto el: bullets)
            SDL_RenderCopy(renderer, debug_texture, NULL, &el.get_pos());

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
