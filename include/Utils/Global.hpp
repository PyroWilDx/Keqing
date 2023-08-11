//
// Created by pyrowildx on 25/06/2023.
//

#ifndef GLOBAL_H
#define GLOBAL_H

#include <unordered_map>
#include <SDL2/SDL_mixer.h>
#include "World/World.hpp"
#include "Utils/Events.hpp"

#define DATA_PATH "data/.dat"
#define DATA_KQ_VOICE_LANG "KeqingVoiceLang"
#define DATA_KQ_COLOR "KeqingColor"
#define DATA_GAME_VOLUME "GameVolume"

class Global {

public:
    Global() = delete;

    static void initGlobal();

    static void cleanUp();

    static void readUserData();

    static void saveUserData(const char *key, std::string &valStr);

    [[nodiscard]] static World *setWorld(int screenW, int screenH,
                                         int backgroundTotalW, int backgroundTotalH,
                                         const char *backgroundImgPath);

    static void deleteWorld();

    static std::unordered_map<std::string, std::string> userData;

    static int windowWidth;
    static int windowHeight;
    static int renderWindowWidth;
    static int renderWindowHeight;
    static bool isWindowFullscreen;

    static bool pressedKeys[KEY_ENUM_N];
    static int pressedTime[KEY_ENUM_N];
    static int lastPressedTime[KEY_ENUM_N];

    static int currentTime;
    static int dt;

    static World *currentWorld;

private:

};

#endif
