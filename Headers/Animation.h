#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED

#include <Engine.h>

class Animation {
public:
    Animation(std::string path, float frameDelay);
    Animation(std::string path, Rectangle cropRegion, float frameDelay);

    ~Animation();

    void Draw(Vector2 position, float animTime);

    void Draw(Vector2 position, float animTime, Color tint);

    Texture2D GetFirstFrame();
private:
    int totalAnimationFrames;
    float frameDelay;

    Texture2D* frames;
};

#endif