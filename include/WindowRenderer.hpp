//
// Created by pyrowildx on 13/05/23.
//

#ifndef WINDOWRENDERER_HPP
#define WINDOWRENDERER_HPP

#include <SDL2/SDL.h>

class Entity;
class Block;

class WindowRenderer {

public:
    ~WindowRenderer();

    static void initWindowRenderer(const char *title, int w, int h);

    static inline WindowRenderer *getInstance() { return instance; }

    static void cleanUp();

    SDL_Texture *loadTexture(const char *imgPath);

    void renderEntity(Entity *entity);

    void display();

    void clear();

    inline SDL_Window *getWindow() { return gWindow; }

    inline SDL_Renderer *getRenderer() { return gRenderer; }

private:
    WindowRenderer(const char *title, int w, int h);

    static WindowRenderer *instance;

    SDL_Window *gWindow;
    SDL_Renderer *gRenderer;

};

#endif
