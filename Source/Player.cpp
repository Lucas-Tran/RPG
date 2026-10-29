#include <Player.h>

#include <Tiles.h>

#include <LevelEditor.h>

#include <Input.h>

Player::Player() {
    position = { 200.0f, 200.0f };
    state = Idle;
    direction = Right;
}

Animation& Player::GetActiveAnimation() const {
    AnimationAllDirections* animationAllDirections;
    switch (state) {
        case Idle: {
            animationAllDirections = &GameAssets::GetInstance().GetPlayerAnimations().GetIdleAnimation();
            break;
        }
        case Running: {
            animationAllDirections = &GameAssets::GetInstance().GetPlayerAnimations().GetRunAnimation();
            break;
        }
        default: {
            animationAllDirections = &GameAssets::GetInstance().GetPlayerAnimations().GetIdleAnimation();
            break;
        }
    }
    switch (direction) {
        case Up: {
            return animationAllDirections->GetUpAnimation();
        }
        case Down: {
            return animationAllDirections->GetDownAnimation();
        }
        case Left: {
            return animationAllDirections->GetLeftAnimation();
        }
        case Right: {
            return animationAllDirections->GetRightAnimation();
        }
        default: {
            return animationAllDirections->GetRightAnimation();
        }
    }
}

Player& Player::GetInstance() {
    static Player instance;
    return instance;
}


void Player::Update() {
    const Vector2 movement = Vector2Scale(Input::GetInstance().GetInputNormalized(), PLAYER_SPEED * GetFrameTime());
    bool canMove = Vector2Length(movement) > 0.0f;
#ifdef ENABLE_LEVEL_EDITOR
    canMove = canMove && LevelEditor::GetInstance().IsMouseOffPalette();
#endif

    if (canMove) {
        SetState(Running);
        position = Vector2Add(position, movement);
        UpdateDirection(movement);
    } else {
        SetState(Idle);
    }
    GetActiveAnimation().Update(GetFrameTime());
}


void Player::UpdateDirection(Vector2 movement) {
    if (movement.x > 0.0f) {
        direction = Right;
        return;
    }
    if (movement.x < 0.0f) {
        direction = Left;
        return;
    }
    if (movement.y > 0.0f) {
        direction = Down;
        return;
    }
    if (movement.y < 0.0f) {
        direction = Up;
        return;
    }
}

void Player::DrawPlayer() const {
    GetActiveAnimation().Draw(Vector2Add(position, { -TILE_SIZE / 2.0f, -TILE_SIZE }));
}

void Player::UpdateCamera(Camera2D& camera) const {
    float offset = 0;
#ifdef ENABLE_LEVEL_EDITOR
    if (LevelEditor::GetInstance().IsEditorEnabled()) {
        float palleteWidth = PALETTE_WIDTH_PIXELS;
        offset = palleteWidth;
    }
#endif
    camera.target = position;
    camera.target.x += offset / 2;
    float screenWidthHalf = static_cast<float>(GetScreenWidth()) / 2;
    float screenHeightHalf = static_cast<float>(GetScreenHeight()) / 2;
    if (camera.target.x < screenWidthHalf) {
        camera.target.x = screenWidthHalf;
    }
    if (camera.target.y < screenHeightHalf) {
        camera.target.y = screenHeightHalf;
    }
    float mapRightEdge = offset + static_cast<float>(Tiles::GetInstance().GetWidth() * TILE_SIZE) - screenWidthHalf;
    if (camera.target.x > mapRightEdge) {
        camera.target.x = mapRightEdge;
    }
    float mapBottomEdge = static_cast<float>(Tiles::GetInstance().GetHeight() * TILE_SIZE) - screenHeightHalf;
    if (camera.target.y > mapBottomEdge) {
        camera.target.y = mapBottomEdge;
    }
}
void Player::SetState(PlayerState newState) {
    PlayerState prevState = state;
    state = newState;
    if (state != prevState) {
        GetActiveAnimation().Reset();
    }
}
