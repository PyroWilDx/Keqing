//
// Created by pyrowildx on 13/07/2023.
//

#ifndef COLORPICKER_HPP
#define COLORPICKER_HPP

#include "UI/Button.hpp"

class VolumeSlider;

class ColorPicker : public Button {

public:
    ColorPicker(double x, double y, int renderW, int renderH, Uint32 currRGBA = 0);

    ~ColorPicker() override;

    void fillPixels(Uint32 currRGBA);

    [[nodiscard]] Uint32 getCurrentRGBA() const;

    [[nodiscard]] Uint32 getCurrentRGBASlided() const;

    void onClick(int mouseX, int mouseY) override;

    void onClickedMove(int mouseX, int mouseY, bool isMouseOnButton) override;

    int getPickerSize();

    void renderSelf(SDL_Renderer *gRenderer) override;

    void setPickerXY(int mouseX, int mouseY);

private:
    Uint32 **absPixels;
    int pickerX, pickerY;
//    VolumeSlider *blackSlider;

};

#endif
