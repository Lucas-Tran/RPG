#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <Engine.h>


#include <GameAssets.h>

enum PlayerState {
    Idle,
    Running
};

enum PlayerDirection {
    Up,
    Down,
    Left,
    Right
};

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


    Animation& GetActiveAnimation() const;

    void SetState(PlayerState newState);

    void UpdateDirection(Vector2 movement);

    void TryMove(Vector2 by);

    Vector2 position;
    PlayerState state;
    PlayerDirection direction;
};

#endif