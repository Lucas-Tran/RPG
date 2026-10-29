#include "Engine.h"
#include <Player.h>
#include <GameAssets.h>
#include <Tiles.h>

#include <MapStorage.h>
#include <LevelEditor.h>

#include <Input.h>


int main() {
    constexpr int screenWidth = 1000;
#ifdef ENABLE_LEVEL_EDITOR
    constexpr int screenHeight = 720;
#else
    constexpr int screenHeight = 450;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
#endif
    InitWindow(screenWidth, screenHeight, "RPG - Raylib Window");

    SetTargetFPS(60);

    SetRandomSeed(42);

    Camera2D camera;
    camera.target = { 0.0f, 0.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    NewMap(Tiles::GetInstance(), 100, 100);

    while (!WindowShouldClose()) {
        Input::GetInstance().Update();
        Player::GetInstance().Update();
        Player::GetInstance().UpdateCamera(camera);
#ifdef ENABLE_LEVEL_EDITOR
        LevelEditor::GetInstance().Update(camera);
#endif

        camera.offset = { static_cast<float>(GetScreenWidth()) / 2.0f, static_cast<float>(GetScreenHeight()) / 2.0f};

        BeginDrawing();

        BeginMode2D(camera);

        Tiles::GetInstance().DrawVisibleRegion(camera, 0);

        Tiles::GetInstance().DrawVisibleRegion(camera, 1);

        Player::GetInstance().DrawPlayer();

        Tiles::GetInstance().DrawVisibleRegion(camera, 2);

#ifdef ENABLE_LEVEL_EDITOR
        LevelEditor::GetInstance().DrawCursor();
#endif

        EndMode2D();

#ifdef ENABLE_LEVEL_EDITOR
        LevelEditor::GetInstance().DrawPallete();
#endif

        EndDrawing();
    }

    GameAssets::GetInstance().UnloadTiles();

    CloseWindow();

    return 0;
}
