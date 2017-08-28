#include <iostream>

#include "grapher.h"

#include <vector>
#include <string>

#define WINDOW_TITLE "2DGrapher"

enum
{
    WINDOW_WIDTH = 1200,
    WINDOW_HEIGHT = 600,
    WINDOW_X = 113,
    WINDOW_Y = 84
};

int main(void)
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), NULL);
        SDL_Quit();
        return -1;
    }

    /*if(TTF_Init() < 0)
    {
        std::cout << "Error: " << TTF_GetError() << std::endl;
    }*/

    SDL_Window *w = SDL_CreateWindow(WINDOW_TITLE, WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT,
                                     SDL_WINDOW_SHOWN);
    SDL_Renderer *r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);

    bool done = false;

    const std::string path("settings.dat");
    Grapher g(r, path);
    const unsigned char *keys = SDL_GetKeyboardState(NULL);
    int x, y, oldX, oldY;
    bool isMoving = false;
    while(!done)
    {
        SDL_Event e;

        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                 case SDL_QUIT:
                    done = true;
                    break;
                 case SDL_MOUSEBUTTONDOWN:
                    isMoving = true;
                    oldX = e.button.x;
                    oldY = e.button.y;
                    break;
                case SDL_MOUSEBUTTONUP:
                    isMoving = false;
                    break;
                case SDL_MOUSEMOTION:
                    if(isMoving)
                    {
                        x = e.button.x;
                        y = e.button.y;
                        g.move(x - oldX, y - oldY);
                        oldX = e.button.x;
                        oldY = e.button.y;
                    }
                    break;
                case SDL_MOUSEWHEEL:
                     if(e.wheel.y < 0)
                         g.zoomOut();
                     else if(e.wheel.y > 0)
                         g.zoomIn();
                    break;
                case SDL_KEYDOWN:
                    if(keys[SDL_SCANCODE_KP_MINUS])
                    {
                        g.zoomOut();
                    }
                    else if(keys[SDL_SCANCODE_KP_PLUS])
                    {
                        g.zoomIn();
                    }
                    else if(keys[SDL_SCANCODE_LEFT])
                    {
                        g.move(Grapher::Direction::LEFT, 0.25);
                    }
                    else if(keys[SDL_SCANCODE_RIGHT])
                    {
                        g.move(Grapher::Direction::RIGHT, 0.25);
                    }
                    else if(keys[SDL_SCANCODE_UP])
                    {
                        g.move(Grapher::Direction::UP, 0.25);
                    }
                    else if(keys[SDL_SCANCODE_DOWN])
                    {
                        g.move(Grapher::Direction::DOWN, 0.25);
                    }

                    break;
            }
        }
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        SDL_RenderClear(r);
        g.draw_all();
        SDL_RenderPresent(r);
    }

    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(w);
    SDL_Quit();

    return 0;
}
