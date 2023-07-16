//
// Created by pyrowildx on 30/05/23.
//

#ifndef ANIMATEDENTITY_HPP
#define ANIMATEDENTITY_HPP

#include "Entity.hpp"
#include "WindowRenderer.hpp"
#include "Utils/SpriteArray.hpp"

class AnimatedEntity : public Entity {

public:
    explicit AnimatedEntity(int spriteArrayLength);

    ~AnimatedEntity() override;

    void setRGBAMod(Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;

    void initSprite(int spriteCode, const char *imgPath,
                    int spriteFrameW, int spriteFrameH, int spriteFrameN,
                    int spriteFrameLength = 0);

    void setSpriteAnimated(bool animated, int spriteCode = 0,
                           void (*preAction)(int spriteCode, void *fParams) = nullptr,
                           void *fParams = nullptr);

    void setSpriteFrameLengthFromTo(int spriteFrameLength, int startFrame, int endFrame,
                                    int spriteCode = 0);

//    void setSpriteFrameLengths(const int *spriteFrameLengths, int spriteCode = 0);

    void setSpriteNext(int nextSpriteCode, int spriteCode);

    bool isSpriteAnimated(int spriteCode = 0);

    bool isFrameAt(int frameIndex, int spriteCode = 0);

    bool isNewestFrame(int frameIndex, int spriteCode = 0);

    bool isFrameBetween(int startFrame, int endFrame, int spriteCode = 0);

    bool willFrameFinish(int frameIndex, int spriteCode = 0);

    bool isCurrentSprite(int spriteCode = 0);

    void goToFrame(int frameIndex, int spriteCode = 0);

    void goToNextFrame(int spriteCode = 0);

    void stopOnFrame(int frameIndex, int spriteCode = 0);

    void pauseSprite(bool pause, int spriteCode = 0);

    void resetSprite(int spriteCode = 0);

    void delaySprite(int ms, int spriteCode = 0);

    void animateSprite();

    inline Sprite *getSprite(int spriteCode = 0) { return (&spriteArray[spriteCode]); }

    inline int getCurrentSpriteCode() { return currentSprite->sCode; }

private:
    SpriteArray spriteArray;
    Sprite *currentSprite;

};

#endif
