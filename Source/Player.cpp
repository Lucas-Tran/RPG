#include <Player.h>

#include <Tiles.h>

#include <LevelEditor.h>

#include <Input.h>

#include <algorithm>

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
        TryMove(movement);
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

char GetSolidAtTile(Rectangle rect, int tileX, int tileY, int layer) {
    char solid = 0;
    short tile = Tiles::GetInstance().GetTile(tileX, tileY, layer);
    int pinNum = 0;
    for (int y = 0; y < PIN_COUNT_ROW; y++) {
        for (int x = 0; x < PIN_COUNT_ROW; x++) {
            float pointX = static_cast<float>(tileX) * TILE_SIZE;
            pointX += static_cast<float>(x) * TILE_SIZE / 2;
            float pointY = static_cast<float>(tileY) * TILE_SIZE;
            pointY += static_cast<float>(y) * TILE_SIZE / 2;
            if (CheckCollisionPointRec({pointX, pointY}, rect)) {
                char pinValue = GameAssets::GetInstance().GetPin(tile, pinNum);
                solid = std::max(solid, pinValue);
            }
            pinNum++;
        }
    }
    return solid;
}

char CheckSolidAllAround(float x, float y, int layer) {
    Rectangle rect = {x - PLAYER_WIDTH_HALF, y - PLAYER_HEIGHT_HALF * 2, PLAYER_WIDTH_HALF * 2, PLAYER_HEIGHT_HALF * 2};
    int tileX = static_cast<int>(floor((x - PLAYER_WIDTH_HALF) / TILE_SIZE));
    int tileY = static_cast<int>(floor(y / TILE_SIZE));
    char solid = 0;
    solid = std::max(solid, GetSolidAtTile(rect, tileX, tileY, layer));
    solid = std::max(solid, GetSolidAtTile(rect, tileX + 1, tileY, layer));
    solid = std::max(solid, GetSolidAtTile(rect, tileX, tileY - 1, layer));
    solid = std::max(solid, GetSolidAtTile(rect, tileX + 1, tileY - 1, layer));
    return solid;
}

char GetSolid(float x, float y) {
    char solid = 0;
    for (int layer = 0; layer < NUM_LAYERS; layer++) {
        solid = std::max(solid, CheckSolidAllAround(x, y, layer));
    }
    return solid;
}

void Player::TryMove(Vector2 by) {
#ifdef ENABLE_LEVEL_EDITOR
    if (LevelEditor::GetInstance().IsEditorEnabled()) {
        constexpr float GOD_SPEED_SCALE = 2;
        position += GOD_SPEED_SCALE * by;
        return;
    }
#endif
    if (GetSolid(position.x + by.x, position.y) != 1) {
        position.x += by.x;
    }
    if (GetSolid(position.x, position.y + by.y) != 1) {
        position.y += by.y;
    }
}
