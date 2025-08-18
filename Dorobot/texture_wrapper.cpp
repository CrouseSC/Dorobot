#include "pch.h"

ImTextureID TextureWrapper::getId() const
{
    return (ImTextureID)id;
}

Animation::Animation()
{
}

Animation::Animation(SpritesheetWrapper* spriteSheet)
{
    this->spriteSheet = spriteSheet;
}

void Animation::setSpriteSheet(SpritesheetWrapper* spriteSheet)
{
    this->spriteSheet = spriteSheet;
}

AnimationFrame Animation::getCurrentFrame()
{
    AnimationFrame animationFrame;
    animationFrame.id = spriteSheet->id;
    //[4, 5]
    int row = currentFrame / spriteSheet->dimensions.y;
    int column = currentFrame % spriteSheet->dimensions.y;
    if (column == 2) {
        volatile int x = 1;
    }
    
    Vec2<float> uv;
    uv.x = column == 0 ? 0 : ( ((float)spriteSheet->size.x / spriteSheet->dimensions.y * column) / spriteSheet->size.x);
    uv.y = row == 0 ? 0 : ( ((float)spriteSheet->size.y / spriteSheet->dimensions.x * row) / spriteSheet->size.y);

    animationFrame.uv = uv;
    animationFrame.size = Vec2<int>(spriteSheet->size.x / 4, spriteSheet->size.y / 4);
    return animationFrame;
}

void Animation::updateAnimation()
{
    float deltaTime = ImGui::GetIO().DeltaTime * 1000;
    timeElapsed += deltaTime;
    
    if (timeElapsed > spriteSheet->delay) {
        timeElapsed = 0;
        currentFrame = (currentFrame + 1) % spriteSheet->frames;
    }
}
