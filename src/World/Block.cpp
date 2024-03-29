//
// Created by pyrowildx on 25/06/2023.
//

#include "World/Block.hpp"
#include "WindowRenderer.hpp"
#include "Utils/Utils.hpp"

Block::Block(int blockCode, double x, double y, int renderW, int renderH)
        : WorldEntity(x, y, renderW, renderH, WORLD_BLOCK) {
    this->blockCode = blockCode;
    this->matchRenderSize = false;
    this->matchRenderSizeOnY = false;
    setBlockInfo();
}

void Block::setBlockInfo() {
    imgFrame.w = 32;
    imgFrame.h = 32;

    const char *imgPath;
    switch (blockCode) {
        case BLOCK_WALL_INVISIBLE:
            imgFrame.w = 0;
            imgFrame.h = 0;
            imgTexture = nullptr;
            return;
        case BLOCK_DIRT:
            imgPath = "res/gfx/block/Dirt.png";
            break;
        case BLOCK_GRASS:
            imgPath = "res/gfx/block/Grass.png";
            break;
        case BLOCK_SNOW:
            imgPath = "res/gfx/block/Snow.png";
            break;
        case BLOCK_QUARTZ:
            imgPath = "res/gfx/block/Quartz.png";
            break;
        case BLOCK_TNT:
            imgPath = "res/gfx/block/TNT.png";
            break;
        default:
            imgPath = "";
            break;
    }

    imgTexture = WindowRenderer::getInstance()->loadTexture(imgPath);
}

void Block::resizeToRenderSize() {
    matchRenderSize = true;
}

void Block::resizeToRenderSizeOnY() {
    matchRenderSizeOnY = true;
}

void Block::renderSelf(SDL_Renderer *gRenderer) {
    SDL_Rect src = this->getFrame();

    if (src.x < 0 || src.y < 0) return;
    if (src.w <= 0 || src.h <= 0) return;

    SDL_Rect dst = this->getRenderRect();
    SDL_Rect blockDst;
    blockDst.x = dst.x;
    blockDst.y = dst.y;
    double xScreenCoeff, yScreenCoeff;
    getScreenXYCoeff(&xScreenCoeff, &yScreenCoeff);
    if (!matchRenderSize) {
        if (!matchRenderSizeOnY) {
            blockDst.w = (int) (imgFrame.w * xScreenCoeff);
            blockDst.h = (int) (imgFrame.h * yScreenCoeff);
            dst.w = (int) (dst.w / xScreenCoeff);
            dst.h = (int) (dst.h / yScreenCoeff);
        } else {
            blockDst.w = dst.h;
            blockDst.h = dst.h;
            dst.w = (int) (dst.w / xScreenCoeff);
            dst.h = src.h;
        }
    } else {
        blockDst.w = dst.w;
        blockDst.h = dst.h;
        dst.w = src.w;
        dst.h = src.h;
    }

    SDL_RendererFlip renderFlip = SDL_FLIP_NONE;
    double renderRotation = this->getRotation();
    if (!this->isFacingEast()) {
        renderFlip = SDL_FLIP_HORIZONTAL;
        renderRotation = -renderRotation;
    }

    int baseX = blockDst.x;
    int baseY = blockDst.y;
    double xCoeff = ((double) dst.w / (double) src.w);
    double yCoeff = ((double) dst.h / (double) src.h);
    for (int i = 0; i < (int) xCoeff; i++) {
        for (int j = 0; j < (int) yCoeff; j++) {
            SDL_RenderCopyEx(gRenderer, this->getTexture(),
                             &src, &blockDst,
                             renderRotation, nullptr, renderFlip);
            blockDst.y += blockDst.h;
        }
        blockDst.y = baseY;
        blockDst.x += blockDst.w;
    }

    if (matchRenderSize) return;

    double xLeftover = xCoeff - (int) xCoeff;
    double yLeftover = yCoeff - (int) yCoeff;
    double epsilonLeftover = 0.01;

    if (xLeftover <= epsilonLeftover && yLeftover <= epsilonLeftover) return;

    int lastX = baseX + blockDst.w * (int) xCoeff;
    int lastY = baseY + blockDst.h * (int) yCoeff;
    int srcBaseW = src.w;
    int srcBaseH = src.h;
    int dstBaseW = blockDst.w;
    int dstBaseH = blockDst.h;
    double srcLeftoverW = (double) srcBaseW * xLeftover;
    double srcLeftoverH = (double) srcBaseH * yLeftover;
    double wLeftoverCoeff = (double) dstBaseW / (double) srcBaseW;
    double hLeftoverCoeff = (double) dstBaseH / (double) srcBaseH;
    if (xLeftover > epsilonLeftover) {
        src.w = (int) srcLeftoverW;
        src.h = srcBaseH;
        blockDst.x = lastX;
        blockDst.y = baseY;
        blockDst.w = (int) (srcLeftoverW * wLeftoverCoeff);
        blockDst.h = dstBaseH;
        for (int j = 0; j < (int) yCoeff; j++) {
            SDL_RenderCopyEx(gRenderer, this->getTexture(),
                             &src, &blockDst,
                             renderRotation, nullptr, renderFlip);
            blockDst.y += blockDst.h;
        }
    }
    if (yLeftover > epsilonLeftover) {
        src.w = srcBaseW;
        src.h = (int) srcLeftoverH;
        blockDst.x = baseX;
        blockDst.y = lastY;
        blockDst.w = dstBaseW;
        blockDst.h = (int) (srcLeftoverH * hLeftoverCoeff);
        for (int i = 0; i < (int) xCoeff; i++) {
            SDL_RenderCopyEx(gRenderer, this->getTexture(),
                             &src, &blockDst,
                             renderRotation, nullptr, renderFlip);
            blockDst.x += blockDst.w;
        }
    }
    if (xLeftover > epsilonLeftover && yLeftover > epsilonLeftover) {
        src.w = (int) srcLeftoverW;
        src.h = (int) srcLeftoverH;
        blockDst.x = lastX;
        blockDst.y = lastY;
        blockDst.w = (int) (srcLeftoverW * wLeftoverCoeff);
        blockDst.h = (int) (srcLeftoverH * hLeftoverCoeff);
        SDL_RenderCopyEx(gRenderer, this->getTexture(),
                         &src, &blockDst,
                         renderRotation, nullptr, renderFlip);
    }
}
