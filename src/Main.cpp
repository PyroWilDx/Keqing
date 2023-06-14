#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <filesystem>
#include <iostream>
#include "Utils.hpp"
#include "WindowRenderer.hpp"
#include "Keqing.hpp"
#include "Text.hpp"
#include "Monster.hpp"
#include "Background.hpp"
#include "MonsterLinkedList.hpp"
#include "Particle.hpp"
#include "Hud.hpp"

// Text constants
const char *fontPath = "res/fonts/JetBrainsMono-Regular.ttf";
const SDL_Color FPSTextColor = {255, 255, 255};
const int FPSFontSize = 40;

int main() {
    std::string path = std::filesystem::current_path();
    std::filesystem::current_path(path + "/..");

    myAssert(SDL_Init(SDL_INIT_VIDEO) >= 0, "SDL_Init FAILED.", SDL_GetError());
    myAssert(IMG_Init(IMG_INIT_PNG) != 0, "IMG_Init FAILED.", SDL_GetError());
    myAssert(TTF_Init() != -1, "TTF_Init FAILED.", SDL_GetError());

    WindowRenderer window = WindowRenderer("Keqing", SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Texture *backgroundTexture = window.loadTexture("res/gfx/Background.png");
    Background background = Background(SCREEN_WIDTH, SCREEN_HEIGHT,
                                       BACKGROUND_WIDTH, backgroundTexture);

    Particle::initParticle(&window);

    Keqing::initKeqing(&window);
    Keqing *kq = Keqing::getInstance();
    kq->moveTo(0, 360);
    kq->colorTexture(4, 1, 4, &window); // TODO
    kq->setRenderWHMultiplier(KQ_WIDTH_MULTIPLIER, KQ_HEIGHT_MULTIPLIER);
    kq->setCollisionRect({(int) (0.0f * KQ_WIDTH_MULTIPLIER),
                          (int) (12.0f * KQ_HEIGHT_MULTIPLIER),
                          (int) (60.0f * KQ_WIDTH_MULTIPLIER),
                          (int) (84.0f * KQ_HEIGHT_MULTIPLIER)});

    const float hudSBCircleM = 3.0f;
    const float hudSBIconM = 2.6f;
    int hudSBCircleX = 1060;
    int hudSBCircleY = 600;
    Hud skillBurstCircleBG = Hud(hudSBCircleX, hudSBCircleY, 64, 32, 32,
                                 window.loadTexture("res/gfx/hud/SkillBurstCircleBG.png"));
    skillBurstCircleBG.setRenderWHMultiplier(hudSBCircleM, hudSBCircleM);
    skillBurstCircleBG.setRGBAMod(255, 255, 255, 128);
    Hud skillBurstCircle = Hud(hudSBCircleX, hudSBCircleY, 64, 32, 32,
                               window.loadTexture("res/gfx/hud/SkillBurstCircle.png"));
    skillBurstCircle.setRenderWHMultiplier(hudSBCircleM, hudSBCircleM);
    Hud skillIcon1 = Hud(0, 0, 21, 21, 21,
                         window.loadTexture("res/gfx/hud/SkillIcon1.png"));
    skillIcon1.setRenderWHMultiplier(hudSBIconM, hudSBIconM);
    skillIcon1.moveToHudFrameCenter(&skillBurstCircle, 0);
    Hud burstIcon = Hud(0, 0, 21, 21, 21,
                        window.loadTexture("res/gfx/hud/BurstIcon.png"));
    burstIcon.setRenderWHMultiplier(hudSBIconM, hudSBIconM);
    burstIcon.moveToHudFrameCenter(&skillBurstCircle, 1);

    auto *monsterLL = new MonsterLinkedList();
//    auto *zombie0 = new Monster(96, 96, &window);
//    zombie0->moveTo(100, DEFAULT_Y, -100);
//    zombie0->setCollisionRect({70, 76, 76, 120});
//    zombie0->setRenderWHMultiplier(MONSTER_WIDTH, MONSTER_HEIGHT);
//    zombie0->walk();
//    monsterLL->insert(zombie0);
//    auto *zombie1 = new Monster(96, 96, &window);
//    zombie1->moveTo(200, DEFAULT_Y, -70);
//    zombie1->setCollisionRect({70, 76, 76, 120});
//    zombie1->setRenderWHMultiplier(MONSTER_WIDTH, MONSTER_HEIGHT);
//    zombie1->run();
//    monsterLL->insert(zombie1);
//    Monster *zombie2 = zombie0->copy(&window);
//    zombie2->moveTo(300, DEFAULT_Y, -50);
//    zombie2->run();
//    monsterLL->insert(zombie2);
//    Monster *zombie3 = zombie0->copy(&window);
//    zombie3->moveTo(400, DEFAULT_Y, -25);
//    zombie3->walk();
//    monsterLL->insert(zombie3);

    SDL_Event event;
    bool pressedKeys[4] = {false, false, false, false};

    int currentTime;
    int dt;
    int lastTime;

    int pressTime;
    int mouseDt;
    bool mousePressed;

    int kqLastX = kq->getX();
    int kqX;

    Text FPSText = Text();
    int accumulatedFPSTime = 1002;
    int accumulatedFrames = 0;

    bool gameRunning = true;
    bool gamePaused = false;
    bool runFrame = false;
    while (gameRunning) {

        // Time Handling
        currentTime = getTime();
        dt = currentTime - lastTime;
        lastTime = currentTime;

        // Events
        int spriteCode = -1;
        int key = -1;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    gameRunning = false;
                    break;
                case SDL_KEYDOWN:
                    key = event.key.keysym.sym;
                    switch (key) {
                        case SDLK_BACKSPACE:
                            gamePaused = !gamePaused;
                            if (!gamePaused) lastTime = getTime();
                            break;
                        case SDLK_RETURN:
                            if (gamePaused) {
                                runFrame = true;
                                dt = 10;
                            }
                            break;
                        case SDLK_q:
                        case SDLK_d:
                            pressedKeys[key % 4] = true;
                            break;
                        case SDLK_SPACE:
                            spriteCode = KQ_JUMP_START;
                            break;
                        case SDLK_LSHIFT:
                            if (!kq->isJumping()) spriteCode = KQ_DASH_START;
                            else spriteCode = KQ_AIR_DASH;
                            break;
                        case SDLK_e: {
                            Particle *idleParticle = Particle::getParticle(PARTICLE_KQ_SKILL_IDLE, 0);
                            if (idleParticle == nullptr) spriteCode = KQ_SKILL;
                            else spriteCode = KQ_SKILL_SLASH;
                        }
                            break;
                        case SDLK_r:
                            spriteCode = KQ_BURST;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    key = event.key.keysym.sym;
                    switch (key) {
                        case SDLK_q:
                        case SDLK_d:
                        case SDLK_s:
                        case SDLK_z:
                            pressedKeys[key % 4] = false;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    pressTime = getTime();
                    mousePressed = true;
                    if (!kq->isJumping()) spriteCode = KQ_NATK;
                    else spriteCode = KQ_AIR_NATK;
                    break;
                case SDL_MOUSEBUTTONUP:
                    mousePressed = false;
                    break;
                default:
                    break;
            }
        }
        if (gamePaused) {
            if (!runFrame) continue;
            else runFrame = false;
        }

//        if (spriteCode == -1) {
//            if (mousePressed) {
//                mouseDt = getTime() - pressTime;
//                if (mouseDt < 200) {
//                    if (!kq->isJumping()) spriteCode = KQ_NATK;
//                    else spriteCode = KQ_AIR_NATK;
//                }
//            }
//        }

        if (spriteCode != -1) {
            if (kq->canDoAction(spriteCode)) {
                kq->preAction(spriteCode);
                kq->setSpriteAnimated(spriteCode, true);
            }
        }

        // Particles
        Particle::animateAll(dt);

        // Keqing
        // TODO Hitlag
        if (kq->canDoAction(KQ_WALK)) kq->updateDirection(pressedKeys, key);
        if (kq->isNAtking()) kq->nAtk(dt, currentTime);
        if (kq->isDashing()) kq->dash();
        if (kq->isSkilling()) kq->skill();
        if (kq->isSkillSlashing()) kq->skillSlash();
        if (kq->isBursting()) kq->burst(dt);
        if (kq->isJumping()) kq->jump(dt);
        if (kq->isAirNAtking()) kq->airNAtk(dt);
        if (kq->isAirDashing()) kq->airDash();
        if (kq->isDamaged()) kq->damage(dt);
        if (kq->isMoving() && !kq->shouldNotMove()) kq->move(dt);

        kqX = kq->getX();
        int xDiff = kqX - kqLastX;
        if (xDiff != 0) background.translate(kq, xDiff);
        kqLastX = kqX;

        kq->animate(dt);

        // Monster(s)
        monsterLL->operateAllCells(&Monster::move, &dt, nullptr);
        if (rand() % 1000 < 10) {
            monsterLL->operateAllCells(&Monster::attack, nullptr, nullptr);
        }
        monsterLL->operateAllCells(&Monster::animate, &dt, nullptr);

        // Handling Keqing Collisions
        bool kqDamaged = false;
        monsterLL->operateAllCells(&Monster::collides, nullptr, &kqDamaged);
        if (kqDamaged) {
            kq->damage(dt); // TODO change
            printf("Noob %d\n", kq->getHp());
        }

        // FPS Text
        if (accumulatedFPSTime > 1000) {
            char text[16];
            sprintf(text, "FPS : %d", accumulatedFrames);
            FPSText.loadTextTexture(0, 0, text, &FPSTextColor,
                                    fontPath, FPSFontSize, window.getRenderer());
            accumulatedFPSTime = 0;
            accumulatedFrames = 0;
        }
        accumulatedFPSTime += dt;
        accumulatedFrames++;

        // Window Rendering
        window.clear();

        window.render(&background, nullptr);
        window.render(&FPSText, nullptr);

        if (sizeof(WindowRenderer) < sizeof(Background)) {
            auto *windowCastBackground = (Background *) &window;
            Background params[2] = {*windowCastBackground, background};
            monsterLL->operateAllCells(&Monster::render, params, nullptr);
        } else {
            auto *backgroundCastWindow = (WindowRenderer *) &background;
            WindowRenderer params[2] = {window, *backgroundCastWindow};
            monsterLL->operateAllCells(&Monster::render, params, nullptr);
        }

        window.render(kq, &background);

        window.render(&skillBurstCircleBG, nullptr);
        window.render(&skillBurstCircle, nullptr);
        window.render(&skillIcon1, nullptr);
        window.render(&burstIcon, nullptr);

        Particle::renderAll(&window, &background);

        window.display();

//        printf("Keqing X : %d\n", kq->getX());
//        printf("Number of Active Particle(s) : %d\n", Particle::getCount());
    }

    // Free
    background.destroy();
    FPSText.destroy();
    monsterLL->deleteAllCells();
    delete monsterLL;
    kq->destroy();
    Particle::cleanUp();
    skillBurstCircleBG.destroy();
    skillBurstCircle.destroy();
    skillIcon1.destroy();
    burstIcon.destroy();
    window.cleanUp();
    SDL_Quit();

    return EXIT_SUCCESS;
}
