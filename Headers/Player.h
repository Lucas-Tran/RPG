#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <Engine.h>


#include <GameAssets.h>

enum PlayerState {
    Idle,
    Running,
    ClimbingUp,
    ClimbingDown
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


    Vector2 GetPosition();
private:
    Player();

    Animation& GetActiveAnimation() const;

    void SetState(PlayerState newState);

    void UpdateDirection(Vector2 movement);

    void CheckOutOfBounds();

    void ChangeScene(Vector2 newPosition, int changeSceneX, int changeSceneY);

    bool IsClimbing();


    Vector2 position;
    PlayerState state;
    PlayerDirection direction;

    float animTime;
};

#endif