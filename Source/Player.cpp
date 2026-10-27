#include <Player.h>

Player::Player() = default;

Player& Player::GetInstance() {
    static Player instance;
    return instance;
}


void Player::Update() {
    const Vector2 movement = Vector2Scale(GetInputNormalized(), PLAYER_SPEED * GetFrameTime());
    position = Vector2Add(position, movement);
}

void Player::DrawPlayer() const {
    DrawRectangleV(position, {50.0f, 50.0f}, BLUE);
}

void Player::UpdateCamera(Camera2D& camera) const {
    camera.target = position;
}

Vector2 Player::GetInputNormalized() const {
    const float horizontalInput = IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT);
    const float verticalInput = IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP);
    Vector2 input = {
        horizontalInput,
        verticalInput
    };
    input = Vector2Normalize(input);
    return input;
}
