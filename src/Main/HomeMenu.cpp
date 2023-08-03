//
// Created by pyrowildx on 09/07/2023.
//

#include "Main/HomeMenu.hpp"
#include "Main/Game1.hpp"
#include "Main/ColorKeqing.hpp"
#include "WindowRenderer.hpp"
#include "Utils/Global.hpp"
#include "Utils/Events.hpp"
#include "Utils/Sound.hpp"

void runHomeMenu() {
    WindowRenderer *gWindow = WindowRenderer::getInstance();

    World *gWorld = Global::setWorld(SCREEN_BASE_WIDTH, SCREEN_BASE_HEIGHT,
                             SCREEN_BASE_WIDTH, SCREEN_BASE_HEIGHT,
                             "res/gfx/background/HomeMenu.png");

    auto *runGame1Button = new Button(400, 0, 200, 100);
    runGame1Button->setCallBack([](Button *self, int mouseX, int mouseY, void *onClickParams) {
        bool *pGRunning = (bool *) onClickParams;
        callMainFunc(pGRunning, &runGame1);
    });
    SDL_Color tmpColor = {255, 255, 255, 255};
    runGame1Button->addText("RRUN GAME 1", &tmpColor, "res/fonts/JetBrainsMono-Regular.ttf", 16);
    runGame1Button->changeColor(255, 0, 0);
    gWorld->addButton(runGame1Button);

    auto *runColorKQButton = new Button(400, 200, 200, 100);
    runColorKQButton->setCallBack([](Button *self, int mouseX, int mouseY, void *onClickParams) {
        bool *pGRunning = (bool *) onClickParams;
        callMainFunc(pGRunning, &runColorKeqing);
    });
    tmpColor = {255, 255, 255, 255};
    runColorKQButton->addText("RRUN CUSTOM MENU 1", &tmpColor, "res/fonts/JetBrainsMono-Regular.ttf", 16);
    runColorKQButton->changeColor(255, 0, 0);
    gWorld->addButton(runColorKQButton);

//    playAudioMusic("res/sound/music/NimbleAsLightning.ogg");

    SDL_Event event;
    gStateInfo gInfo = DEFAULT_GAME_STATE_INFO;
    runGame1Button->setOnClickParams((void *) &(gInfo.gRunning));
    runColorKQButton->setOnClickParams((void *) &(gInfo.gRunning));

    while (gInfo.gRunning) {

        while (SDL_PollEvent(&event)) {
            handleBasicEvents(&event, nullptr, &gInfo);
        }
        if (!gInfo.gRunning) break;

        gWorld->onGameFrame();

        gWindow->clear();

        gWorld->renderSelf();

        gWindow->display();
    }
}
