#ifndef __MAIN_CLASS_FOR_GAME_HPP__
#define __MAIN_CLASS_FOR_GAME_HPP__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL_ttf.h>

using namespace std;

class Bullet {
private:
    SDL_Rect position;
    float dx, dy;  // velocity
public:
    Bullet(SDL_Rect position_, double dx_, double dy_) {
        position = position_;
        dx = dx_;
        dy = dy_;
    }

    void Update() {
        position.x += dx;
        position.y += dy;
    }

    SDL_Rect& get_pos() {
        return position;
    }
};

class Ship {
private:
    double dx, dy;  // velocity
    SDL_Rect position;
    int hull;
public:
    Ship(SDL_Rect position_, double dx_, double dy_) {
        position = position_;
        dx = dx_;
        dy = dy_;
        hull = 100;
    }

    void move(double dx_, double dy_)
    {
        position.x += dx_;
        position.y += dy_;
    }

    SDL_Rect& get_position()
    {
        return position;
    }

    int get_hull()
    {
        return hull;
    }

    void set_hull(int hull_)
    {
        hull = hull_;
    }

    bool is_ship_destroyed()
    {
        if (hull <= 0)
        {
            cout<<"Your ship has sinked"<<endl;
            return true;
        } 
        else return false;
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

void render_text(SDL_Renderer &renderer, TTF_Font &used_font, int text_position_x, int text_position_y, string text)
{
    const SDL_Color font_color = {255, 255, 255};
    SDL_Surface * surface = TTF_RenderText_Solid(&used_font, text.c_str(), font_color);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(&renderer, surface);
    SDL_FreeSurface(surface);

    int texW = 0;
    int texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect text_position = {text_position_x, text_position_y, texW, texH};

    SDL_RenderCopy(&renderer, texture, NULL, &text_position);  // draw texture on screen
    SDL_DestroyTexture(texture);
}

#endif