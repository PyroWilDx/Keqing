//
// Created by pyrowildx on 10/07/2023.
//

#include "Main/ColorKeqing.hpp"
#include "Utils/Global.hpp"
#include "Utils/Events.hpp"
#include "Keqing.hpp"
#include "UI/ColorPicker.hpp"

void runColorKeqing() {
    WindowRenderer *gWindow = WindowRenderer::getInstance();

    auto *gWorld = new World(SCREEN_BASE_WIDTH, SCREEN_BASE_HEIGHT,
                             SCREEN_BASE_WIDTH, SCREEN_BASE_HEIGHT,
                             "res/gfx/background/ColorKeqing.png");
    Global::currentWorld = gWorld;

    auto *colorPicker = new ColorPicker(600, 200, 300, 400);
    colorPicker->setCallBack([](Button *self, int mouseX, int mouseY, void *onClickParams) {
        auto *selfColorPicker = (ColorPicker *) self;
        Uint32 rgba = selfColorPicker->getCurrentRGBA();
        Keqing::getInstance()->colorTexture(rgba >> 24, rgba >> 16, rgba >> 8);
    });
    gWorld->addButton(colorPicker);

    // TODO FAIRE UN BOUTON SET COLOR POUR EVITER LE LAG.

    Keqing *kq = Keqing::getInstance();
    kq->moveTo(100, 100);
    kq->setRenderWHMultiplier(KQ_WIDTH_MULTIPLIER, KQ_HEIGHT_MULTIPLIER);
    kq->setHitbox({(int) (0.0 * KQ_WIDTH_MULTIPLIER),
                   (int) (12.0 * KQ_HEIGHT_MULTIPLIER),
                   (int) (60.0 * KQ_WIDTH_MULTIPLIER),
                   (int) (84.0 * KQ_HEIGHT_MULTIPLIER)});

    SDL_Event event;
    bool gRunning = true;

    while (gRunning) {

        handleTime();

        while (SDL_PollEvent(&event)) {
            handleBasicEvents(&event, &gRunning);
        }

        kq->animateSprite();

        gWindow->clear();

        gWorld->renderSelf();

        gWindow->renderEntity(kq);

        gWindow->display();
    }
}