//
// Created by pyrowildx on 24/07/2023.
//

#ifndef SLIME_HPP
#define SLIME_HPP

#include <string>
#include "BaseEntity/Monster.hpp"

enum {
    SLIME_IDLE,
    SLIME_WALK,
    SLIME_CROUCH,
    SLIME_JUMP,
    SLIME_ATK,
    SLIME_DEATH,
    SLIME_ENUM_N
};

class Slime : public Monster {

public:
    explicit Slime(const std::string &colorString);

    bool onGameFrame() override;

    void AI() override;

    void hurt() override;

    void updateAction() override;

private:

};

#endif
