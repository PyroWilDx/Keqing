#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <filesystem>
#include <iostream>
#include "Utils.hpp"
#include "WindowRenderer.hpp"
#include "Player.hpp"
#include "Text.hpp"
#include "Monster.hpp"
#include "Background.hpp"
#include "MonsterLinkedList.hpp"

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

    WindowRenderer window = WindowRenderer("FirstSDL2", SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Texture *backgroundTexture = window.loadTexture("res/gfx/background.png");
    Background background = Background(SCREEN_WIDTH, SCREEN_HEIGHT,
                                       3000, backgroundTexture);

    Player player = Player(80, 80, &window);
    player.moveTo(0, DEFAULT_Y, 0);
    player.setCollisionRect({40, 60, 60, 90});
    player.setRenderWH(PLAYER_WIDTH, PLAYER_HEIGHT);

    auto *monsterLL = new MonsterLinkedList();
    auto *zombie0 = new Monster(96, 96, &window);
    zombie0->moveTo(100, DEFAULT_Y, -100);
    zombie0->setCollisionRect({70, 76, 76, 120});
    zombie0->setRenderWH(MONSTER_WIDTH, MONSTER_HEIGHT);
    zombie0->walk();
    monsterLL->insert(zombie0);
    auto *zombie1 = new Monster(96, 96, &window);
    zombie1->moveTo(200, DEFAULT_Y, -70);
    zombie1->setCollisionRect({70, 76, 76, 120});
    zombie1->setRenderWH(MONSTER_WIDTH, MONSTER_HEIGHT);
    zombie1->run();
    monsterLL->insert(zombie1);
    Monster *zombie2 = zombie0->copy(&window);
    zombie2->moveTo(300, DEFAULT_Y, -50);
    zombie2->run();
    monsterLL->insert(zombie2);
    Monster *zombie3 = zombie0->copy(&window);
    zombie3->moveTo(400, DEFAULT_Y, -25);
    zombie3->walk();
    monsterLL->insert(zombie3);

    Uint32 dt;
    Uint32 lastTime = SDL_GetTicks();
    Uint32 currentTime;

    Text FPSText = Text();
    Uint32 accumulatedFPSTime = 10000;
    Uint32 accumulatedFrames = 0;

    SDL_Event event;
    int key;
    bool keyPressed[4] = {false, false, false, false};

    bool gameRunning = true;
    while (gameRunning) {

        // Events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    gameRunning = false;
                    break;
                case SDL_KEYDOWN:
                    key = event.key.keysym.sym;
                    switch (key) {
                        case SDLK_q:
                        case SDLK_d:
                        case SDLK_s:
                        case SDLK_z:
                            keyPressed[key % 4] = true;
                            player.updateDirection(key, keyPressed);
                            break;
                        case SDLK_SPACE:
                            player.setTextureAnimated(PLAYER_JUMP_SPRITE, true);
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
                            keyPressed[key % 4] = false;
                            player.clearDirection(key, keyPressed);
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    player.attack();
                    break;
                default:
                    break;
            }
        }

        // Time Handling
        currentTime = SDL_GetTicks();
        dt = currentTime - lastTime;
        lastTime = currentTime;

        // Player
        if (player.isMoving()) {
            player.move((int) dt);
            int playerX = player.getX() + player.getCollisionRect().x;
            int playerW = player.getCollisionRect().w;
            int halfX = SCREEN_WIDTH / 2 - playerW / 2;
            int playerXDirection = player.getXDirection();
            if ((playerXDirection == 1 && playerX > halfX) ||
                (playerXDirection == -1 && playerX < halfX)) {
                int lastX = background.getFrame().x;
                background.move((int) dt, player.getXDirection());
                int translateX = lastX - background.getFrame().x;
                // Translate all Entities
                player.addX(translateX);
                monsterLL->operateAllCells(&Monster::addX, &translateX, nullptr);
            }
        }
        if (player.isJumping()) player.jump((int) dt);
        if (player.isDamaged()) player.damage((int) dt);
        bool animated = player.animate((int) dt);

        // Monster(s)
        monsterLL->operateAllCells(&Monster::move, &dt, nullptr);
        if (rand() % 1000 < 10) {
            monsterLL->operateAllCells(&Monster::attack, nullptr, nullptr);
        }
        monsterLL->operateAllCells(&Monster::animate, &dt, nullptr);

        // Handling Player Collisions
        bool playerDamaged = false;
        monsterLL->operateAllCells(&Monster::collides, &player, &playerDamaged);
        if (playerDamaged) {
            player.damage((int) dt);
            printf("Noob %d\n", player.getHp());
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

        window.clear();
        window.render(&background);
        window.render(&FPSText);
        monsterLL->operateAllCells(&Monster::render, &window, nullptr);
        window.render(&player);
        window.display();
    }

    // Free
    background.destroy();
    FPSText.destroy();
    monsterLL->deleteAllCells();
    delete monsterLL;
    player.destroy();
    window.cleanUp();
    SDL_Quit();

    return EXIT_SUCCESS;
}