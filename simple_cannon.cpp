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

class Bullet {
private:
    SDL_Point position;
    double dx, dy;  // velocity
public:
    Bullet(SDL_Point position, double dx, double dy, SDL_Renderer *renderer) {
        this->position = position;
        this->dx = dx;
        this->dy = dy;
    }

    void Update() {

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

void draw_cannon(SDL_Renderer &renderer, SDL_Point cannonStandPosition, SDL_Point cannonBarrelPosition, int angle)
{
    SDL_Point cannon_rotated_point = rotate_point(cannonStandPosition.x, cannonStandPosition.y, angle, cannonBarrelPosition);  // point rotated by certain angle
    SDL_SetRenderDrawColor(&renderer, 0, 255, 0, 255);  
    SDL_RenderDrawLine(&renderer, cannonStandPosition.x, cannonStandPosition.y, cannon_rotated_point.x, cannon_rotated_point.y);  
}

void draw_bullets(SDL_Renderer &renderer, vector<Bullet> activeBullets)
{
    SDL_SetRenderDrawColor(&renderer, 255, 0, 0, 255);
    for (Bullet b : activeBullets)
    {
        SDL_RenderDrawPoint(&renderer, b.getPosition().x, b.getPosition().y);
    }
}

void update_bullets(vector<Bullet> &activeBullets)  // simple bullets physics
{
    for (Bullet &b : activeBullets)
    {
        SDL_Point pos = b.getPosition();



        SDL_Point newPos = {int(pos.x), int(pos.y + b.getVelocityY())};
        b.setPosition(newPos);

        cout << "X: "<<newPos.x<<" Y: "<<newPos.y<<endl;
    }
}

void delete_non_active_bullets(vector<Bullet> &activeBullets)
{
    int counter = 0;
    for (Bullet &b : activeBullets)
    {
        auto pos = b.getPosition();
        if (pos.x < 0 || pos.x > width || pos.y < 0 || pos.y > height)  // bullet is out of the screen
        {
            activeBullets.erase(activeBullets.begin() + counter);
        }
        counter++;
    }
}

int angleLimiter(int angle)  // limits angle to value from 0 to 360
{
    if (angle > 360 || angle < 0) {
        angle = 0;
    }
    return angle;
}

Bullet shootBullet(SDL_Point initialPoint, SDL_Renderer *renderer)
{
    double dx = 10;
    double dy = 5;
    Bullet newBullet(initialPoint, dx, dy, renderer);
    return newBullet;
}

int main(int, char **)
{
    int xMouse, yMouse;
    int angle = 0;
    SDL_Point cannonStandPosition = {250, 250};  // starting point of cannon line
    SDL_Point cannonBarrelPosition = {150, 150};  // end point of cannon line
    vector<Bullet> activeBullets;

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

    // draw cannon
    draw_cannon(*renderer, cannonStandPosition, cannonBarrelPosition, angle);

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
                    angle++;
                    break;
                case SDLK_DOWN:
                    cout << "Down" << endl;
                    angle--;
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
                    SDL_Point initialBulletPosition = rotate_point(cannonStandPosition.x, cannonStandPosition.y, angle, cannonBarrelPosition);
                    activeBullets.push_back(shootBullet(initialBulletPosition, renderer));  // shoots bullet and saves it to active bullets
                    cout << "Active bullets: "<< activeBullets.size() <<endl;
                    break;
                }

                // clear map
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(renderer);

                // draw objects
                draw_cannon(*renderer, cannonStandPosition, cannonBarrelPosition, angle);
                draw_bullets(*renderer, activeBullets);

                // apply physics to bullets
                update_bullets(activeBullets);
                delete_non_active_bullets(activeBullets);
                
                // limit angle to values from 0 to 360
                angle = angleLimiter(angle);
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
