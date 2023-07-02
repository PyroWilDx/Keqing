//
// Created by pyrowildx on 13/05/23.
//

#include <vector>
#include <SDL2/SDL_image.h>
#include "Keqing.hpp"
#include "Particle.hpp"
#include "Global.hpp"

#define SKILL_TP_DISTANCE 400

Keqing *Keqing::instance = nullptr;

Keqing::Keqing()
        : AnimatedEntity(KQ_ENUM_N) {
    gravityWeight = 0.0024;
    hp = 1;

    initSprite(KQ_IDLE, "res/gfx/keqing/Idle.png",
               96, 96, 18, 60);
    setSpriteAnimated(KQ_IDLE, true);
    setSpriteNext(KQ_IDLE, KQ_IDLE);

    initSprite(KQ_JUMP_END, "res/gfx/keqing/JumpEnd.png",
               96, 96, 2, 100);

    initSprite(KQ_WALK, "res/gfx/keqing/Walk.png",
               96, 96, 8, 60);
    setSpriteNext(KQ_WALK, KQ_WALK);

    initSprite(KQ_TURN, "res/gfx/keqing/Turn.png",
               96, 96, 3, 40);

    initSprite(KQ_NATK, "res/gfx/keqing/NAtk.png",
               192, 160, 34, 60);

    initSprite(KQ_DASH, "res/gfx/keqing/Dash.png",
               128, 96, 9, 30);
    setSpriteFrameLengthFromTo(KQ_DASH, 42, 2, 4); // Invisible
    setSpriteFrameLengthFromTo(KQ_DASH, 54, 6, -1); // End

    initSprite(KQ_SKILL, "res/gfx/keqing/Skill.png",
               128, 96, 13, 60);

    initSprite(KQ_SKILL_SLASH, "res/gfx/keqing/SkillSlash.png",
               192, 128, 14, 60);

    initSprite(KQ_BURST, "res/gfx/keqing/Burst.png",
               128, 96, 30, 70);
    setSpriteFrameLengthFromTo(KQ_BURST, INT32_MAX, 13);

    initSprite(KQ_JUMP_START, "res/gfx/keqing/JumpStart.png",
               96, 96, 2, 20);
    setSpriteNext(KQ_JUMP_START, KQ_JUMP);

    initSprite(KQ_JUMP, "res/gfx/keqing/Jump.png",
               96, 128, 7, 80);
    setSpriteFrameLengthFromTo(KQ_JUMP, INT32_MAX, 2, 4);
    setSpriteNext(KQ_JUMP, KQ_JUMP_END);

    initSprite(KQ_AIR_DASH, "res/gfx/keqing/AirDash.png",
               96, 96, 6, 40);

    initSprite(KQ_AIR_NATK, "res/gfx/keqing/AirNAtk.png",
               128, 128, 14, 60);

    initSprite(KQ_HURT, "res/gfx/keqing/Hurt.png",
               96, 96, 6, 60);

    texture = spriteArray[KQ_IDLE].sTexture;
}


void Keqing::initKeqing() {
    if (instance == nullptr) {
        instance = new Keqing();
    } else {
        SDL_Log("Keqing already initialized!\n");
    }
}


void Keqing::colorTexture(int r, int g, int b) {
    SDL_Surface *img = IMG_Load("res/gfx/keqing/Idle.png");

    auto *pixel0 = (Uint8 *) img->pixels;
    for (Uint8 *pixel = pixel0; pixel < pixel0 + (img->w * img->h) * 4; pixel += 4) {

        double h, s, v;
        RGBtoHSV(pixel[0], pixel[1], pixel[2],
                 &h, &s, &v);

        // Hair (Green)
        if (h >= 120 && h <= 180) {
            h = 280 + (h - 120);
        }

        // Outfit & Gloves (Red)
        if ((h >= 330 && h <= 360) || (h >= 0 && h <= 30)) {
            if (h >= 330 && h <= 360)
                h = 280 + (h - 330);
            else if (h >= 0 && h <= 30)
                h = 280 + (h - 0);
        }

        HSVtoRGB(h, s, v,
                 &pixel[0], &pixel[1], &pixel[2]);
    }

    SDL_Texture *mTexture = SDL_CreateTextureFromSurface(
            WindowRenderer::getInstance()->getRenderer(), img);
    spriteArray[KQ_IDLE].sTexture = mTexture;
}


SDL_Rect Keqing::getRenderRect() {
    SDL_Rect dst = Entity::getRenderRect();
    dst.x += (int) ((96.0 - (double) frame.w) / 2.0);
    dst.y += (int) ((96.0 - (double) frame.h) / 2.0);
    return dst;
}


void Keqing::updateDirection() {
    if (Global::pressedKeys[KEY_Q] && Global::pressedKeys[KEY_D]) {
        setFacingEast(!facingEast);
        xVelocity = 0;
    } else {
        if (Global::pressedKeys[KEY_Q]) {
            setFacingEast(false);
            xVelocity = KQ_WALK_VELOCITY;
        } else if (Global::pressedKeys[KEY_D]) {
            setFacingEast(true);
            xVelocity = KQ_WALK_VELOCITY;
        } else {
            xVelocity = 0;
        }
    }

    if (xVelocity != 0) {
        setSpriteAnimated(KQ_WALK, true);
    } else {
        setSpriteAnimated(KQ_WALK, false);
    }
}


const int NAtkMax = 4;
const int NAtkEndFrame[NAtkMax] = {3, 9, 17, 28};
const double NAtkVelocity[NAtkMax + 1] = {0.1, 0, 0.2,
                                          0, 0.4};

void Keqing::NAtk() {
    // TODO CHANGE DIRECTION
    for (int i = 0; i < NAtkMax; i++) {
        if (willFrameFinish(KQ_NATK, NAtkEndFrame[i] + 1)) {
            if (!isMouseLeftRecent()) { // Stop NAtk
                setSpriteAnimated(KQ_NATK, false);
                return;
            }
        }

        if (isFrameBetween(KQ_NATK, NAtkEndFrame[i], NAtkEndFrame[i] + 1)) {
            if (isMouseLeftRecent()) { // Continue NAtk
                goToFrame(KQ_NATK, NAtkEndFrame[i] + 2);
                xVelocity = NAtkVelocity[i + 1];
            }
        }
    }

    if (isNewestFrame(KQ_NATK, 0)) {
        xVelocity = NAtkVelocity[0];

    } else if (isNewestFrame(KQ_NATK, 30)) { // NAtk 4
        Particle *NAtk4Particle =
                Particle::push(PARTICLE_KQ_NATK_4, 60, 2.4, 2.6);
        NAtk4Particle->setEntity(this);
        NAtk4Particle->shiftXY(32, 20);
    }
}


void Keqing::dash() {
    if (isNewestFrame(KQ_DASH, 0)) {
        xVelocity = 0;

    } else if (isNewestFrame(KQ_DASH, 2)) {
        xVelocity = -KQ_DASH_VELOCITY;

    } else if (isFrameBetween(KQ_DASH, 6, -1)) {
        if (xVelocity < 0) {
            xVelocity += 0.01 * (double) Global::dt;
        }
    }
}


Particle *newSkillCircleHud = nullptr;

static void skillTimerHudOnRemove(Particle *removedParticle) {
    Particle::pushFast(newSkillCircleHud);
    newSkillCircleHud->setRGBAMod(KQ_SKILL_CIRCLE_RGBA);
    Particle *skillIcon1Hud = Particle::getParticle(PARTICLE_HUD_SKILL_ICON_1, 0);
    skillIcon1Hud->setRGBAMod(RGBA_FULL);
}

int skillUseTime;

static void skillIcon2HudOnRemove(Particle *removedParticle) {
    Particle *skillCircleHud = Particle::getParticle(PARTICLE_HUD_SKILL_CIRCLE, 0);
    newSkillCircleHud = skillCircleHud->copy();
    newSkillCircleHud->setRGBAMod(KQ_SKILL_CIRCLE_RGBA);

    Particle *skillIcon1 =
            Particle::push(PARTICLE_HUD_SKILL_ICON_1,
                           INT32_MAX,
                           HUB_SB_ICON_M, HUB_SB_ICON_M);
    skillIcon1->moveToEntityCenter(skillCircleHud);
    skillIcon1->setRGBAMod(RGB_FULL, HUD_SB_USED_ALPHA);

    int elapsedTime = getTime() - skillUseTime;
    Particle *timerHud =
            Particle::push(PARTICLE_HUD_SKILL_BURST_TIMER,
                           (KQ_SKILL_COOLDOWN - elapsedTime) / HUD_SB_TIMER_FRAME_N,
                           HUD_SB_CIRCLE_M * 1.0,
                           HUD_SB_CIRCLE_M * 1.0);
    timerHud->moveToEntityCenter(skillCircleHud);
    timerHud->setOnRemove(&skillTimerHudOnRemove);

    Particle::remove(PARTICLE_HUD_SKILL_CIRCLE, 0);

    Particle::remove(PARTICLE_KQ_SKILL_IDLE, 0);
}

double lightningStelittoXY[2];

void Keqing::ESkill() {
    if (isNewestFrame(KQ_SKILL, 0)) {
        skillUseTime = getTime();

        Particle *skillCircleHud = Particle::getParticle(PARTICLE_HUD_SKILL_CIRCLE, 0);
        Particle *skillIcon2 =
                Particle::push(PARTICLE_HUD_SKILL_ICON_2,
                               KQ_LIGHTNING_STILETTO_DURATION,
                               HUB_SB_ICON_M, HUB_SB_ICON_M);
        skillIcon2->moveToEntityCenter(skillCircleHud);
        skillIcon2->setOnRemove(&skillIcon2HudOnRemove);
        Particle::remove(PARTICLE_HUD_SKILL_ICON_1, 0);
        skillCircleHud->setRGBAMod(238, 10, 238, 255);

    } else if (isNewestFrame(KQ_SKILL, 6)) { // Lightning Stiletto
        Particle *spawnParticle =
                Particle::push(PARTICLE_KQ_SKILL_SPAWN,
                               60, 1, 1);
        spawnParticle->moveToEntityCenter(this);
        double distance = SKILL_TP_DISTANCE;
        if (!facingEast) distance = -distance;
        spawnParticle->moveTo(this, distance, 0);
        lightningStelittoXY[0] = x + distance;
        lightningStelittoXY[1] = y;
    }

    Particle *spawnParticle = Particle::getParticle(PARTICLE_KQ_SKILL_SPAWN, 0);
    if (spawnParticle != nullptr) { // Lightning Stiletto Idle
        if (spawnParticle->isNewestFrame(PARTICLE_KQ_SKILL_SPAWN, 6)) {
            Particle *idleParticle =
                    Particle::push(PARTICLE_KQ_SKILL_IDLE,
                                   200, 2.0, 2.0);
            idleParticle->moveToEntityCenter(spawnParticle);
        }
    }
}


void Keqing::ESkillSlash() {
    if (isNewestFrame(KQ_SKILL_SLASH, 0)) {
        Particle *idleParticle = Particle::getParticle(PARTICLE_KQ_SKILL_IDLE, 0);
        if (x < idleParticle->getX()) setFacingEast(true);
        else setFacingEast(false);

    } else if (isNewestFrame(KQ_SKILL_SLASH, 6)) {
        Particle *tpStartParticle =
                Particle::push(PARTICLE_KQ_BURST_VANISH,
                               60, 1.0, 1.0);
        tpStartParticle->moveToEntityCenter(this);

        this->moveTo(lightningStelittoXY[0], lightningStelittoXY[1]);

        Particle *tpEndParticle = Particle::push(PARTICLE_KQ_SKILL_TP_END,
                                                 60, 0.64, 0.64);
        tpEndParticle->moveToEntityCenter(this);

        Particle::remove(PARTICLE_HUD_SKILL_ICON_2, 0);
    }
}


const int cSlashFrameDuration = 16;
const int cSlashRotations[KQ_BURST_NUMBER_OF_CLONE_SLASH] =
        {16, 200, 354, 152, 306, 90};
const double cSlashXShift[KQ_BURST_NUMBER_OF_CLONE_SLASH] =
        {72, 0, -4, -10, -78, 0};
const double cSlashYShift[KQ_BURST_NUMBER_OF_CLONE_SLASH] =
        {26, 0, -72, 0, -30, -14};
const double cSlashWM[KQ_BURST_NUMBER_OF_CLONE_SLASH] =
        {0.88, 1.6, 1.2, 1.52, 1.32, 1.0};
const double cSlashHM[KQ_BURST_NUMBER_OF_CLONE_SLASH] =
        {1.0, 1.2, 1.2, 1.2, 1.2, 1.2};
int cSlashCount = 0;

static void pushCloneSlashParticle(Particle *removedParticle) {
    if (cSlashCount < KQ_BURST_NUMBER_OF_CLONE_SLASH) {
        auto *cSlashParticle =
                Particle::push(PARTICLE_KQ_BURST_CLONE_SLASH,
                               cSlashFrameDuration,
                               cSlashWM[cSlashCount],
                               cSlashHM[cSlashCount]);
        cSlashParticle->moveToEntityCenter(Keqing::getInstance());
        cSlashParticle->shiftXY(cSlashXShift[cSlashCount], cSlashYShift[cSlashCount]);
        cSlashParticle->setRotation(cSlashRotations[cSlashCount]);
        cSlashParticle->setOnRemove(&pushCloneSlashParticle);
        cSlashCount++;
    } else {
        cSlashCount = 0;
    }
}

const int slashDuration = 46;
const int slashRotations[KQ_BURST_NUMBER_OF_SLASH] =
        {0, 20, 40, 60, 80, 100, 120, 140};

static void pushSlashParticle() {
    Particle *aoeParticle = Particle::getParticle(PARTICLE_KQ_BURST_AOE, 0);
    for (int i = 0; i < KQ_BURST_NUMBER_OF_SLASH; i++) {
        Particle *slashParticle =
                Particle::push(PARTICLE_KQ_BURST_SLASH,
                               slashDuration,
                               1.2, 1.2);
        slashParticle->moveToEntityCenter(aoeParticle);
        slashParticle->setRotation(slashRotations[i]);
        slashParticle->delay(PARTICLE_KQ_BURST_SLASH, 2 * slashDuration * i);
    }
}

const int cVanishDuration = 40;
const double cVanishXShift[KQ_BURST_NUMBER_OF_CLONE] =
        {100, -80, 74, -100, -14};
const double cVanishYShift[KQ_BURST_NUMBER_OF_CLONE] =
        {26, -36, -48, 36, -80};

static void pushCloneVanishParticle(Particle *aoeParticle) {
    aoeParticle = Particle::getParticle(PARTICLE_KQ_BURST_CLONE, 0);
    for (int i = 0; i < KQ_BURST_NUMBER_OF_CLONE; i++) {
        Particle *cloneVanishParticle =
                Particle::push(PARTICLE_KQ_BURST_CLONE_VANISH,
                               cVanishDuration,
                               1.2, 1.2);
        cloneVanishParticle->moveToEntityCenter(aoeParticle);
        cloneVanishParticle->shiftXY(cVanishXShift[i], cVanishYShift[i]);
    }
}

const double aoeBaseWHM = 0;
const double aoeMaxWHM = 2.0;
const int cAppearDuration = 48;
const double cAppearXShift[KQ_BURST_NUMBER_OF_CLONE] =
        {142, -120, 98, -136, -16};
const double cAppearYShift[KQ_BURST_NUMBER_OF_CLONE] =
        {42, -40, -58, 58, -110};

void Keqing::RBurst() {
    if (isNewestFrame(KQ_BURST, 0)) { // Burst Start
        setFacingEast(true);

        Particle::remove(PARTICLE_HUD_BURST_CIRCLE, 0);
        Particle *burstIconHud = Particle::getParticle(PARTICLE_HUD_BURST_ICON, 0);
        burstIconHud->setRGBAMod(RGB_FULL, HUD_SB_USED_ALPHA);

        Particle::push(PARTICLE_KQ_BURST_AOE,
                       80, aoeBaseWHM, aoeBaseWHM);

        Particle *aoeWaveParticle =
                Particle::push(PARTICLE_KQ_BURST_AOE_WAVE,
                               60, aoeBaseWHM, aoeBaseWHM);
        aoeWaveParticle->setRGBAMod(RGB_FULL, 128);

    } else if (isNewestFrame(KQ_BURST, 12)) { // Vanish
        Particle *vanishParticle =
                Particle::push(PARTICLE_KQ_BURST_VANISH,
                               60, 1.0, 1.0);
        vanishParticle->setEntity(this);

    } else if (isNewestFrame(KQ_BURST, 13)) { // Clone Slash Start
        pushCloneSlashParticle(nullptr);
        Particle *cloneParticle =
                Particle::push(PARTICLE_KQ_BURST_CLONE,
                               (int) (cSlashFrameDuration * 1.8),
                               2.0, 2.0);
        cloneParticle->delay(PARTICLE_KQ_BURST_CLONE, cSlashFrameDuration);
        cloneParticle->moveToEntityCenter(this);
        cloneParticle->shiftXY(0, -46);
        cloneParticle->stopOnFrame(cloneParticle->getCode());

    } else if (isNewestFrame(KQ_BURST, 28)) { // Final Slash
        Particle *aoeParticle = Particle::getParticle(PARTICLE_KQ_BURST_AOE, 0);
        Particle *aoeWaveParticle = Particle::getParticle(PARTICLE_KQ_BURST_AOE_WAVE, 0);

        Particle *finalSlashParticle =
                Particle::push(PARTICLE_KQ_BURST_FINAL_SLASH,
                               80, 1.0, 1.0);
        finalSlashParticle->moveToEntityCenter(aoeParticle);

        pushCloneVanishParticle(aoeParticle);

        aoeParticle->fadeAway();
        aoeWaveParticle->fadeAway();

        Particle::remove(PARTICLE_KQ_BURST_CLONE, 0);
    }

    // Enlarge Circle
    Particle *aoeParticle = Particle::getParticle(PARTICLE_KQ_BURST_AOE, 0);
    if (aoeParticle != nullptr) {
        if (aoeParticle->getRenderWMultiplier() < aoeMaxWHM) {
            double addWHValue = 0.004 * (double) Global::dt;
            aoeParticle->addRenderWHMultiplier(addWHValue, addWHValue,
                                               aoeMaxWHM, aoeMaxWHM);
            aoeParticle->moveToEntityCenter(this);
            Particle *aoeWaveParticle = Particle::getParticle(PARTICLE_KQ_BURST_AOE_WAVE, 0);
            aoeWaveParticle->addRenderWHMultiplier(addWHValue, addWHValue,
                                                   aoeMaxWHM, aoeMaxWHM);
            aoeWaveParticle->moveToEntityCenter(this);
        }
    }

    if (isFrameAt(KQ_BURST, 13)) {
        Particle *cloneParticle = Particle::getParticle(PARTICLE_KQ_BURST_CLONE, 0);
        for (int i = 0; i < KQ_BURST_NUMBER_OF_CLONE; i++) {
            // 0, 5, 10, 15, 20
            if (cloneParticle->isNewestFrame(PARTICLE_KQ_BURST_CLONE, 5 * i)) {
                Particle *cloneAppearParticle =
                        Particle::push(PARTICLE_KQ_BURST_CLONE_APPEAR,
                                       cAppearDuration,
                                       1.0, 1.0);
                cloneAppearParticle->moveToEntityCenter(this);
                cloneAppearParticle->shiftXY(cAppearXShift[i], cAppearYShift[i]);
            }
        }

        if (!Particle::isActive(PARTICLE_KQ_BURST_CLONE_SLASH, 0)) { // Clone Slash End
            goToNextFrame(KQ_BURST);
            pushSlashParticle();
        }
    }
}


const double lowFallVelocity = 0.1;
const double averageFallVelocity = 0.2;

void Keqing::airAnimate() {
    if (yVelocity == 0) {
        setSpriteAnimated(KQ_JUMP, false);
        return;
    }

    setSpriteAnimated(KQ_JUMP, true);

    if (yVelocity < 0) {
        if (yVelocity < -averageFallVelocity) {
            if (willFrameFinish(KQ_JUMP, 1)) {
                goToFrame(KQ_JUMP, 0);
            }
        } else if (yVelocity < -lowFallVelocity) {
            goToFrame(KQ_JUMP, 2);
        } else {
            goToFrame(KQ_JUMP, 3);
        }
    }

    if (yVelocity > 0) {
        if (yVelocity < lowFallVelocity) {
            goToFrame(KQ_JUMP, 3);
        } else if (yVelocity < averageFallVelocity) {
            goToFrame(KQ_JUMP, 4);
        } else {
            if (isFrameBetween(KQ_JUMP, 0, 4)) {
                goToFrame(KQ_JUMP, 5);
            }
            if (willFrameFinish(KQ_JUMP, -1)) {
                goToFrame(KQ_JUMP, 5);
            }
        }
    }
}


void Keqing::jump() {
    if (isFrameBetween(KQ_JUMP, 0, 3)) {
        yVelocity += gravityWeight * (double) Global::dt;
        yVelocity -= gravityWeight * (double) Global::dt; // Cancel Gravity
    }
}


void Keqing::airNAtk() { // Plunge Attack in Genshin
    if (isNewestFrame(KQ_AIR_NATK, 0)) {
        setSpriteAnimated(KQ_JUMP, false);
        yVelocity = 0.01;

    } else if (isNewestFrame(KQ_AIR_NATK, 1)) {
        yVelocity = -0.4;

    } else if (isNewestFrame(KQ_AIR_NATK, 6)) {
        Particle *airNAttackParticle =
                Particle::push(PARTICLE_KQ_AIR_NATK,
                               60, 3.2, 2.0);
        airNAttackParticle->setEntity(this);
        airNAttackParticle->shiftXY(24, -24);
        yVelocity = 0.2;

    }

    if (willFrameFinish(KQ_AIR_NATK, 7)) {
        if (yVelocity != 0) {
            goToFrame(KQ_AIR_NATK, 6);
        }
    }

    if (isFrameBetween(KQ_AIR_NATK, 6, 7)) {
        if (yVelocity == 0) {
            goToFrame(KQ_AIR_NATK, 8);
            Particle::remove(PARTICLE_KQ_AIR_NATK, 0);
            Particle *airNAtkGroundParticle =
                    Particle::push(PARTICLE_KQ_AIR_NATK_GROUND,
                                   60, 1.0, 1.0);
            airNAtkGroundParticle->moveToEntityCenter(this);
            airNAtkGroundParticle->shiftXY(20, -6);
        }
    }

}


void Keqing::airDash() {
    xVelocity = KQ_AIR_DASH_VELOCITY;
}


void Keqing::damage() { // TODO change
    if (!isDamaged()) {
        setSpriteAnimated(KQ_HURT, true);
        x -= 10;
        hp--;
    } else {
        x -= KQ_KNOCKBACK_VELOCITY * (double) Global::dt;
    }
}


void Keqing::setFacingEast(bool value) {
    if (facingEast != value) {
        setSpriteAnimated(KQ_TURN, true);
        facingEast = value;
    }
}


static bool checkNextSprites(int spriteCode, Sprite *spriteArray) {
    for (int i = spriteCode + 1; i < KQ_ENUM_N; i++) {
        if (spriteArray[i].sAnimated) return false;
    }
    return true;
}

bool Keqing::canDoAction(int spriteCode) {
    if (spriteCode == KQ_WALK) {
        for (int i = KQ_WALK + 1; i < KQ_ENUM_N; i++) {
            if (i == KQ_JUMP) continue;
            if (spriteArray[i].sAnimated) return false;
        }
        return true;
    }

    if (spriteArray[spriteCode].sAnimated) return false;

    switch (spriteCode) {
        case KQ_SKILL:
            if (Particle::getParticle(PARTICLE_KQ_SKILL_SPAWN, 0) != nullptr) return false;
            if (Particle::getParticle(PARTICLE_HUD_SKILL_CIRCLE, 0) == nullptr) return false;
            goto finally;

        case KQ_BURST:
            if (spriteArray[KQ_SKILL].sAnimated) return false;
            if (spriteArray[KQ_SKILL_SLASH].sAnimated) return false;
            if (Particle::getParticle(PARTICLE_HUD_BURST_CIRCLE, 0) == nullptr) return false;
            goto finally;

        case KQ_JUMP_START:
            if (spriteArray[KQ_DASH].sAnimated) return false;
            if (spriteArray[KQ_SKILL].sAnimated) return false;
            if (spriteArray[KQ_SKILL_SLASH].sAnimated) return false;
            if (spriteArray[KQ_BURST].sAnimated) return false;
            goto finally;

        default:
            goto finally;

        finally:
            return checkNextSprites(spriteCode, spriteArray);
    }
}

void Keqing::preAction(int spriteCode) {
    // TODO BASICALLY PUT EVERY ISNEWESTFRAME 0 HERE
    switch (spriteCode) {
        case KQ_DASH:
            if (Global::pressedKeys[KEY_Q]) setFacingEast(true);
            else if (Global::pressedKeys[KEY_D]) setFacingEast(false);
            break;

        case KQ_SKILL:
        case KQ_SKILL_SLASH:
        case KQ_BURST:
            xVelocity = 0;
            break;

        case KQ_JUMP_START:
            yVelocity = -KQ_JUMP_BASE_VELOCITY;
            break;

        default:
            break;
    }
}
