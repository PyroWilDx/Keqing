//
// Created by pyrow on 31/07/2023.
//

#include <cstdio>
#include "Entity/FPSText.hpp"
#include "Utils/Global.hpp"

const char *FPSFontPath = "res/fonts/JetBrainsMono-Regular.ttf";
const SDL_Color FPSTextColor = {COLOR_WHITE_FULL};
const int FPSFontSize = 40;

FPSText::FPSText()
        : Text(0, 0, "FPS : 0", &FPSTextColor,
               FPSFontPath, FPSFontSize, false) {
    accumulatedFPSTime = 0;
    accumulatedFrames = 0;
}

void FPSText::onGameFrame() {
    if (accumulatedFPSTime > 1000) {
        char text[16];
        sprintf(text, "FPS : %d", accumulatedFrames);
        changeText(text);
        accumulatedFPSTime = 0;
        accumulatedFrames = 0;
    }
    accumulatedFPSTime += Global::dt;
    accumulatedFrames++;
}
