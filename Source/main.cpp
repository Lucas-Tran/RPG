#include "Engine.h"
#include <Player.h>
#include <GameAssets.h>
#include <Tiles.h>
#include <Animation.h>

#include <MapStorage.h>

void GetCameraUpdateInput(Camera2D& camera) {
    float deltaTile = GetFrameTime();
    if (IsKeyDown(KEY_W)) {
        camera.target.y -= 100 * deltaTile;
    }
    if (IsKeyDown(KEY_S)) {
        camera.target.y += 100 * deltaTile;
    }
    if (IsKeyDown(KEY_A)) {
        camera.target.x -= 100 * deltaTile;
    }
    if (IsKeyDown(KEY_D)) {
        camera.target.x += 100 * deltaTile;
    }
}


int main() {
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "RPG - Raylib Window");

    SetTargetFPS(60);

    Camera2D camera;
    camera.target = { 0.0f, 0.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    NewMap(Tiles::GetInstance(), 100, 100);

    while (!WindowShouldClose()) {
        Player::GetInstance().Update();
        Player::GetInstance().UpdateCamera(camera);
        GetCameraUpdateInput(camera);

        camera.offset = { static_cast<float>(GetScreenWidth()) / 2.0f, static_cast<float>(GetScreenHeight()) / 2.0f};

        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode2D(camera);

        Tiles::GetInstance().DrawVisibleRegion(camera);

        Player::GetInstance().DrawPlayer();



        EndMode2D();

        EndDrawing();
    }

    GameAssets::GetInstance().UnloadTiles();

    CloseWindow();

    return 0;
}
