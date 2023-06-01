//
// Created by pyrowildx on 13/05/23.
//

#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SDL2/SDL.h>

class Entity {

public:
    Entity(int x, int y, int z);

    Entity(int x, int y, int z, int w, int h, bool hasShadow, SDL_Texture *texture);

    void move(int dt, float speed);

    void moveTo(int x_, int y_, int z_);

    bool collides(Entity *entity);

    void clearTexture();

    virtual void destroy();

    inline void setFrame(SDL_Rect frame_) { frame = frame_; }

    inline void setCollisionRect(SDL_Rect collisionRect_) { collisionRect = collisionRect_; }

    inline void setRenderWH(int w_, int h_) {
        renderW = w_;
        renderH = h_;
    }

    inline int getX() { return x; }

    inline int getY() { return y; }

    inline int getZ() { return z; }

    inline int getXDirection() { return xDirection; }

    inline SDL_Rect getFrame() { return frame; }

    inline SDL_Rect getCollisionRect() { return collisionRect; }

    inline bool getHasShadow() { return hasShadow; }

    inline SDL_Texture *getTexture() { return texture; }

    inline int getRenderW() { return renderW; }

    inline int getRenderH() { return renderH; }

protected:
    int x, y, z;
    int xDirection, yDirection, zDirection;
    SDL_Rect frame;
    SDL_Rect collisionRect;
    bool hasShadow;
    SDL_Texture *texture;
    int renderW, renderH;

private:

};

#endif
