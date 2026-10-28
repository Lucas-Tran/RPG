#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED

#include <Engine.h>

class Animation {
public:
    Animation(std::string path, float frameDelay);

    ~Animation();

    void Reset();

    void Update(float deltaTime);

    void Draw(Vector2 position);
private:
    int totalAnimationFrames;

    float timer;
    float frameDelay;

    Texture2D* frames;
};

#endif