#include <LevelEditor.h>

#ifdef ENABLE_LEVEL_EDITOR

#include <GameAssets.h>

#include <Tiles.h>

#include <Input.h>

#include <MapStorage.h>

#include <algorithm>

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
    pinEditorEnabled = false;

    selectedPalleteIndex = 0;

    time = 0;

    layer = 0;

    palleteGrid = new short[TILESET_SIZE];
    int tileNum = 0;
    int i = 0;
    for (int j = 0; j < OVERWORLD_TILESET_WIDTH * OVERWORLD_TILESET_HEIGHT; j++) {
        palleteGrid[i] = tileNum;
        i++;
        tileNum++;
    }
    for (int y = 0; y < DUNGEON_TILESET_HEIGHT; y++) {
        for (int j = 0; j < DUNGEON_TILESET_WIDTH; j++) {
            palleteGrid[i] = tileNum;
            i++;
            tileNum++;
        }
        for (int j = 0; j < TILESET_WIDTH - DUNGEON_TILESET_WIDTH; j++) {
            palleteGrid[i] = EMPTY_TILE;
            i++;
        }
    }
}

LevelEditor::~LevelEditor() {
    delete[] palleteGrid;
}

LevelEditor& LevelEditor::GetInstance() {
    static LevelEditor instance;
    return instance;
}

void LevelEditor::SetLayer(int layer) {
    if (pinEditorEnabled && !IsMouseOffPalette()) {
        return;
    }
    if (this->layer == layer) {
        onionModeEnabled = !onionModeEnabled;
    } else {
        this->layer = layer;
    }
}

void LevelEditor::Update(Camera2D camera) {
    if (IsKeyPressed(KEY_P)) {
        pinEditorEnabled = !pinEditorEnabled;
    }
    if (IsKeyPressed(KEY_ZERO)) {
        if (editorEnabled) {
            SaveScene();
            GameAssets::GetInstance().SaveTilePins();
            onionModeEnabled = false;
            pinEditorEnabled = false;
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
            selectedPalleteIndex += cursorPositionX - prevCursorPositionX;
            selectedPalleteIndex += (cursorPositionY - prevCursorPositionY) * TILESET_WIDTH;
            if (selectedPalleteIndex < 0) {
                selectedPalleteIndex = 0;
            } else if (selectedPalleteIndex >= TILESET_SIZE) {
                selectedPalleteIndex = TILESET_SIZE - 1;
            }
        }
        if (IsKeyDown(KEY_E)) {
            short tile = Tiles::GetInstance().GetTile(cursorPositionX, cursorPositionY, layer);
            selectedPalleteIndex = std::find(palleteGrid, palleteGrid + TILESET_SIZE, tile) - palleteGrid;
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            short tile = palleteGrid[selectedPalleteIndex];
            Tiles::GetInstance().SetTile(cursorPositionX, cursorPositionY, layer, tile);
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
            short tile = palleteGrid[selectedPalleteIndex];
        if (tile != EMPTY_TILE) {
            DrawTextureV(GameAssets::GetInstance().GetTile(tile), drawPosition, {255, 255, 255, 160});
        }
        DrawTextureV(outlineTexture, drawPosition - Vector2{ cursorThickness, cursorThickness }, WHITE);
    } else {

    }
}


constexpr Color PIN_OFF = {255, 255, 255, 160};

void DrawPins(float x, float y, short tile) {
    float startX = x + TILE_SIZE / 2 - COLLISION_PIN_DISTANCE;
    float startY = y + TILE_SIZE / 2 - COLLISION_PIN_DISTANCE;
    int pinNumber = 0;
    y = startY;
    for (int iy = 0; iy < PIN_COUNT_ROW; iy++) {
        x = startX;
        for (int ix = 0; ix < PIN_COUNT_ROW; ix++) {
            if (Vector2Distance(GetMousePosition(), {x, y}) < COLLISION_PIN_RADIUS) {
                if (IsKeyDown(KEY_ONE)) {
                    GameAssets::GetInstance().SetPin(tile, pinNumber, 1);
                }
                if (IsKeyDown(KEY_X)) {
                    GameAssets::GetInstance().SetPin(tile, pinNumber, 0);
                }
            }
            Color pinColor = GameAssets::GetInstance().GetPin(tile, pinNumber) ? BLUE : PIN_OFF;
            DrawCircle(x, y, COLLISION_PIN_RADIUS, pinColor);
            pinNumber++;
            x += COLLISION_PIN_DISTANCE;
        }
        y += COLLISION_PIN_DISTANCE;
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
            int palleteIndex = tileGridY * TILESET_WIDTH + tileGridX;
            float tileX = startX + PADDING + x * TILE_SIZE;
            float tileY = startY + PADDING + y * TILE_SIZE;
            Rectangle box = { tileX, tileY, TILE_SIZE, TILE_SIZE };
            if (CheckCollisionPointRec(GetMousePosition(), box) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                selectedPalleteIndex = palleteIndex;
            }
            // Check if the tile index is the selected tile
            if (palleteIndex == selectedPalleteIndex) {
                cursorX = tileX;
                cursorY = tileY;
                showCursor = true;
            }
            DrawTexture(maskTexture, tileX, tileY, WHITE);
            short tile = palleteGrid[palleteIndex];
            if (tile >= 0) {
                DrawTexture(GameAssets::GetInstance().GetTile(tile), tileX, tileY, WHITE);
            }
            if (pinEditorEnabled) {
                DrawPins(tileX, tileY, tile);
            }
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
