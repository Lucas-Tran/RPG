#include <Player.h>

#include <Tiles.h>

#include <LevelEditor.h>

#include <Input.h>

#include <algorithm>
#include <sstream>

#include "MapStorage.h"

struct TileCollisionInfo {
    bool solid = false;
    char maxPin = 0;
    int tileIndex;
};

Player::Player() {
    animTime = 0;
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
        case ClimbingUp: {
            return GameAssets::GetInstance().GetPlayerAnimations().GetClimbUpAnimation();
        }
        case ClimbingDown: {
            return GameAssets::GetInstance().GetPlayerAnimations().GetClimbDownAnimation();
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

void TryMove(Vector2& position, Vector2 by, TileCollisionInfo& infoHorizontal, TileCollisionInfo& infoVertical);

void Player::Update() {
    Vector2 movement = Vector2Scale(Input::GetInstance().GetInputNormalized(), PLAYER_SPEED * GetFrameTime());
    bool canMove = Vector2Length(movement) > 0.0f;
#ifdef ENABLE_LEVEL_EDITOR
    canMove = canMove && LevelEditor::GetInstance().IsMouseOffPalette();
#endif

    if (canMove) {
        if (IsClimbing() && (movement.y != 0)) {
            movement.y *= CLIMB_SPEED_SLOWDOWN;
            animTime += GetFrameTime();
        }
        TileCollisionInfo infoHorizontal;
        TileCollisionInfo infoVertical;
        TryMove(position, movement, infoHorizontal, infoVertical);
        if (infoHorizontal.maxPin == PIN_CHANGE_SCENE) {
            std::string sceneName = Tiles::GetInstance().GetTileDataString(infoHorizontal.tileIndex, SLOT_SCENE_NAME);
            SetSceneName(sceneName);
            GameManager::GetInstance().SetCutsceneFunction(LoadScene);
        }
        if (infoVertical.maxPin == PIN_CLIMBABLE) {
            if (movement.y < 0) {
                SetState(ClimbingUp);
            } else {
                SetState(ClimbingDown);
            }
            int gridX = static_cast<int>(floor(position.x / TILE_SIZE));
            position.x = (static_cast<float>(gridX) + 0.5f) * TILE_SIZE;
        } else {
            SetState(Running);
        }
        UpdateDirection(movement);
    } else {
        if (!IsClimbing()) {
            SetState(Idle);
        }
    }
    if (!IsClimbing()) {
        animTime += GetFrameTime();
    }
    CheckOutOfBounds();
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
    GetActiveAnimation().Draw(Vector2Add(position, { -TILE_SIZE / 2.0f, -TILE_SIZE }), animTime);
}

void Player::UpdateCamera(Camera2D& camera) const {
    float offset = 0;
#ifdef ENABLE_LEVEL_EDITOR
    if (LevelEditor::GetInstance().IsPalleteOnly()) {
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
        animTime = 0;
    }
}

void GetSolidAtTile(Rectangle rect, int tileX, int tileY, TileCollisionInfo &info) {
    int pinNum = 0;
    for (int y = 0; y < PIN_COUNT_ROW; y++) {
        for (int x = 0; x < PIN_COUNT_ROW; x++) {
            float pointX = static_cast<float>(tileX) * TILE_SIZE;
            pointX += static_cast<float>(x) * TILE_SIZE / 2;
            float pointY = static_cast<float>(tileY) * TILE_SIZE;
            pointY += static_cast<float>(y) * TILE_SIZE / 2;
            if (CheckCollisionPointRec({pointX, pointY}, rect)) {
                bool solid = false;
                bool ignore = false;
                for (int layer = 0; layer < NUM_LAYERS; layer++) {
                    short tile = Tiles::GetInstance().GetTile(tileX, tileY, layer);
                    char pinValue = GameAssets::GetInstance().GetPin(tile, pinNum);
                    if (pinValue == PIN_SOLID) {
                        solid = true;
                    }
                    if (pinValue == PIN_IGNORE || pinValue == PIN_CLIMBABLE) {
                        ignore = true;
                    }
                    info.maxPin = std::max(info.maxPin, pinValue);
                    info.tileIndex = Tiles::GetInstance().GetWidth() * tileY + tileX;
                }
                solid = solid && !ignore;
                info.solid = info.solid || solid;
            }
            pinNum++;
        }
    }
}

void GetSolid(float x, float y, TileCollisionInfo &info) {
    Rectangle rect = {x - PLAYER_WIDTH_HALF, y - PLAYER_HEIGHT_HALF * 2, PLAYER_WIDTH_HALF * 2, PLAYER_HEIGHT_HALF * 2};
    int tileX = static_cast<int>(floor((x - PLAYER_WIDTH_HALF) / TILE_SIZE));
    int tileY = static_cast<int>(floor(y / TILE_SIZE));
    GetSolidAtTile(rect, tileX, tileY, info);
    GetSolidAtTile(rect, tileX + 1, tileY, info);
    GetSolidAtTile(rect, tileX, tileY - 1, info);
    GetSolidAtTile(rect, tileX + 1, tileY - 1, info);
}

void TryMove(Vector2& position, Vector2 by, TileCollisionInfo& infoHorizontal, TileCollisionInfo& infoVertical) {
#ifdef ENABLE_LEVEL_EDITOR
    if (LevelEditor::GetInstance().IsEditorEnabled()) {
        constexpr float GOD_SPEED_SCALE = 2;
        position += GOD_SPEED_SCALE * by;
        return;
    }
#endif
    GetSolid(position.x + by.x, position.y, infoHorizontal);
    if (!infoHorizontal.solid) {
        position.x += by.x;
    }
    GetSolid(position.x, position.y + by.y, infoVertical);
    if (!infoVertical.solid) {
        position.y += by.y;
    }
}

void Player::ChangeScene(Vector2 newPosition, int changeSceneX, int changeSceneY) {
#ifdef ENABLE_LEVEL_EDITOR
    if (LevelEditor::GetInstance().IsEditorEnabled()) {
        SaveScene();
    }
#endif
    std::stringstream stream;
    stream.str(GetSceneName());
    std::string enviornmentType;
    int sceneX, sceneY;
    getline(stream, enviornmentType, '_');

    char tmp;

    stream >> sceneX >> tmp >> sceneY;

    sceneX += changeSceneX;
    sceneY += changeSceneY;


    stream.str("");
    stream.clear();

    stream << enviornmentType << '_' << sceneX << '_' << sceneY;
    SetSceneName(stream.str());

    auto sceneTransition = [this, newPosition]() {
        LoadScene();
        position = newPosition;
    };

    GameManager::GetInstance().SetCutsceneFunction(sceneTransition);
}

void Player::CheckOutOfBounds() {
    float LEFT_EDGE = PLAYER_WIDTH_HALF;
    float RIGHT_EDGE = Tiles::GetInstance().GetWidth() * TILE_SIZE - PLAYER_WIDTH_HALF;
    float TOP_EDGE = PLAYER_HEIGHT_HALF;
    float BOTTOM_EDGE = Tiles::GetInstance().GetHeight() * TILE_SIZE;
    if (position.x < LEFT_EDGE) {
        ChangeScene({RIGHT_EDGE, position.y}, -1, 0);
    }
    if (position.y < TOP_EDGE) {
        ChangeScene({position.x, BOTTOM_EDGE}, 0, -1);
    }
    if (position.x > RIGHT_EDGE) {
        ChangeScene({LEFT_EDGE, position.y}, 1, 0);
    }
    if (position.y > BOTTOM_EDGE) {
        ChangeScene({position.x, TOP_EDGE}, 0, 1);
    }
}

bool Player::IsClimbing() {
    return state == ClimbingUp || state == ClimbingDown;
}
