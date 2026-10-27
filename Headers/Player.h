#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <Engine.h>

class Player {
public:
    static Player& GetInstance();
    void Update();
    void DrawPlayer() const;
    void UpdateCamera(Camera2D& camera) const;

    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

private:
    Player();
    Vector2 GetInputNormalized() const;

    Vector2 position = {0.0f, 0.0f};
};

#endif