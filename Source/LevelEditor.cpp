#include <LevelEditor.h>

#ifdef ENABLE_LEVEL_EDITOR

#include <GameAssets.h>

#include <Tiles.h>

#include <Input.h>

#include <MapStorage.h>

LevelEditor::LevelEditor() {
    Image outlineImage = LoadImage("Editor/Outline.png");
    float scaleFactor = static_cast<float>(TILE_SIZE) / 32.0f;
    ImageResizeNN(&outlineImage, static_cast<int>(outlineImage.width * scaleFactor), static_cast<int>(outlineImage.height * scaleFactor));
    outlineTexture = LoadTextureFromImage(outlineImage);
    Image maskImage = LoadImage("Editor/Mask.png");
    scaleFactor = static_cast<float>(TILE_SIZE) / 32.0f;
    ImageResizeNN(&maskImage, static_cast<int>(maskImage.width * scaleFactor), static_cast<int>(maskImage.height * scaleFactor));
    maskTexture = LoadTextureFromImage(maskImage);
    UnloadImage(maskImage);

    editorEnabled = false;
    onionModeEnabled = false;

    selectedTile = 0;

    time = 0;

    layer = 0;
}

LevelEditor& LevelEditor::GetInstance() {
    static LevelEditor instance;
    return instance;
}

void LevelEditor::SetLayer(int layer) {
    if (this->layer == layer) {
        onionModeEnabled = !onionModeEnabled;
    } else {
        this->layer = layer;
    }
}

void LevelEditor::Update(Camera2D camera) {
    if (IsKeyPressed(KEY_ZERO)) {
        if (editorEnabled) {
            SaveScene();
            onionModeEnabled = false;
        }
        editorEnabled = !editorEnabled;
    }
    if (!editorEnabled) {
        return;
    }

    if (IsKeyPressed(KEY_ONE)) {
        SetLayer(0);
    }
    if (IsKeyPressed(KEY_TWO)) {
        SetLayer(1);
    }
    if (IsKeyPressed(KEY_THREE)) {
        SetLayer(2);
    }

    Vector2 cursorPosition = GetScreenToWorld2D(GetMousePosition(), camera);
    if (IsMouseOffPalette()) {
        int prevCursorPositionX = cursorPositionX;
        int prevCursorPositionY = cursorPositionY;
        cursorPositionX = static_cast<int>(floor(cursorPosition.x / TILE_SIZE));
        cursorPositionY = static_cast<int>(floor(cursorPosition.y / TILE_SIZE));
        if (IsKeyDown(KEY_SPACE)) {
            selectedTile += cursorPositionX - prevCursorPositionX;
            selectedTile += (cursorPositionY - prevCursorPositionY) * TILESET_WIDTH;
            if (selectedTile < 0) {
                selectedTile = 0;
            } else if (selectedTile >= TILESET_SIZE) {
                selectedTile = TILESET_SIZE - 1;
            }
        }
        if (IsKeyDown(KEY_E)) {
            selectedTile = Tiles::GetInstance().GetTile(cursorPositionX, cursorPositionY, layer);
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Tiles::GetInstance().SetTile(cursorPositionX, cursorPositionY, layer, selectedTile);
        }
        time = 0;
    } else {
        time += GetFrameTime();
        constexpr float PALETE_SCROLL_DELAY = 0.1f;
        if (time >= PALETE_SCROLL_DELAY) {
            palleteScrollX += Input::GetInstance().GetJoystickX();
            if (palleteScrollX < 0) {
                palleteScrollX = 0;
            } else if (palleteScrollX > TILESET_WIDTH - PALETTE_WIDTH) {
                palleteScrollX = TILESET_WIDTH - PALETTE_WIDTH;
            }
            palleteScrollY += Input::GetInstance().GetJoystickY();
            if (palleteScrollY < 0) {
                palleteScrollY = 0;
            } else if (palleteScrollY > TILESET_HEIGHT - PALETTE_HEIGHT) {
                palleteScrollY = TILESET_HEIGHT - PALETTE_HEIGHT;
            }
            time = 0;
        }
    }
}

constexpr float cursorThickness = TILE_SIZE / 32.0f * 2;

void LevelEditor::DrawCursor() {
    if (!editorEnabled) {
        return;
    }
    if (IsMouseOffPalette()) {
        Vector2 drawPosition = { static_cast<float>(cursorPositionX * TILE_SIZE), static_cast<float>(cursorPositionY * TILE_SIZE) };
        if (selectedTile != EMPTY_TILE) {
            DrawTextureV(GameAssets::GetInstance().GetTile(selectedTile), drawPosition, {255, 255, 255, 160});
        }
        DrawTextureV(outlineTexture, drawPosition - Vector2{ cursorThickness, cursorThickness }, WHITE);
    } else {

    }
}

void LevelEditor::DrawPallete() {
    if (!editorEnabled) {
        return;
    }
    float rectWidth = PALETTE_WIDTH_PIXELS;
    float rectHeight = PALETTE_HEIGHT * TILE_SIZE + (PADDING * 2);
    float startX = GetScreenWidth() - rectWidth;
    float startY = 0;
    DrawRectangle(startX, startY, rectWidth, rectHeight, BLACK);

    float cursorX, cursorY;
    bool showCursor = false;

    for (int y = 0; y < PALETTE_HEIGHT; ++y) {
        for (int x = 0; x < PALETTE_WIDTH; ++x) {
            // Calculate the tile index based on the x and y position in the palette
            int tileGridX = (x + palleteScrollX);
            int tileGridY = (y + palleteScrollY);
            int tileIndex = tileGridY * TILESET_WIDTH + tileGridX;
            float tileX = startX + PADDING + x * TILE_SIZE;
            float tileY = startY + PADDING + y * TILE_SIZE;
            Rectangle box = { tileX, tileY, TILE_SIZE, TILE_SIZE };
            if (CheckCollisionPointRec(GetMousePosition(), box) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                selectedTile = tileIndex;
            }
            // Check if the tile index is the selected tile
            if (tileIndex == selectedTile) {
                cursorX = tileX;
                cursorY = tileY;
                showCursor = true;
            }
            DrawTexture(maskTexture, tileX, tileY, WHITE);
            DrawTexture(GameAssets::GetInstance().GetTile(tileIndex), tileX, tileY, WHITE);
        }
    }
    if (showCursor) {
        DrawTexture(outlineTexture, cursorX - cursorThickness, cursorY - cursorThickness, WHITE);
    }

    DrawText(("Layer: " + std::to_string(layer + 1)).c_str(), 25, 25, 50, BLACK);
}

bool LevelEditor::IsEditorEnabled() {
    return editorEnabled;
}

bool LevelEditor::IsMouseOffPalette() {
    return !editorEnabled || GetMouseX() < GetScreenWidth() - PALETTE_WIDTH_PIXELS;
}

int LevelEditor::GetLayer() {
    return layer;
}

bool LevelEditor::IsOnionModeEnabled() {
    return onionModeEnabled;
}

#endif
