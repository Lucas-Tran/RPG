#include <Tiles.h>

#include <Engine.h>
#include <GameAssets.h>
#include <LevelEditor.h>

Tiles::Tiles() = default;

Tiles::~Tiles() {
    delete[] map;
}

Tiles& Tiles::GetInstance() {
    static Tiles instance;
    return instance;
}

void Tiles::Init(int width, int height) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Map width and height must be positive");
    }

    const std::size_t mapWidth = static_cast<std::size_t>(width);
    const std::size_t mapHeight = static_cast<std::size_t>(height);
    if (mapWidth > std::numeric_limits<std::size_t>::max() / mapHeight) {
        throw std::length_error("Map dimensions are too large");
    }

    delete[] map;
    map = new short[mapWidth * mapHeight * NUM_LAYERS];
    this->width = width;
    this->height = height;
}

short Tiles::GetTile(int index) {
    if (map == nullptr) {
        throw std::logic_error("Map has not been initialized");
    }
    if (index < 0 || static_cast<std::size_t>(index) >=
                         static_cast<std::size_t>(width) * height * NUM_LAYERS) {
        throw std::out_of_range("Tile index is outside the map");
    }
    return map[index];
}

void Tiles::SetTile(int index, short tile) {
    if (map == nullptr) {
        throw std::logic_error("Map has not been initialized");
    }
    if (index < 0 || static_cast<std::size_t>(index) >=
                         static_cast<std::size_t>(width) * height * NUM_LAYERS) {
        throw std::out_of_range("Tile index is outside the map");
    }
    map[index] = tile;
}

short Tiles::GetTile(int x, int y, int layer) {
    if (map == nullptr) {
        throw std::logic_error("Map has not been initialized");
    }
    if (!IsTileInBounds(x, y, layer)) {
        return EMPTY_TILE;
    }
    int index = layer * GetLayerSize() + y * width + x;
    return map[index];
}

void Tiles::SetTile(int x, int y, int layer, short tile) {
    if (map == nullptr) {
        throw std::logic_error("Map has not been initialized");
    }
    if (!IsTileInBounds(x, y, layer)) {
        throw std::out_of_range("Tile coordinates or layer are outside the map");
    }
    int index = layer * GetLayerSize() + y * width + x;
    map[index] = tile;
}

int Tiles::GetWidth() {
    return width;
}

int Tiles::GetHeight() {
    return height;
}

constexpr short BACKGROUND_TILE = 24;

constexpr Color ONION_MODE_TINT = {255, 255, 255, 70};

void Tiles::DrawVisibleRegion(Camera2D& camera2D, int layer) {
    int screenWidthTiles = GetScreenWidth() / TILE_SIZE + 2;
    int screenHeightTiles = GetScreenHeight() / TILE_SIZE + 2;
    int drawOffsetX = -static_cast<int>(camera2D.offset.x);
    int drawOffsetY = -static_cast<int>(camera2D.offset.y);
    int gridX = static_cast<int>(floor((camera2D.target.x + drawOffsetX) / TILE_SIZE));
    int gridY = static_cast<int>(floor((camera2D.target.y + drawOffsetY) / TILE_SIZE));
    for (int y = gridY; y < gridY + screenHeightTiles; y++) {
        for (int x = gridX; x < gridX + screenWidthTiles; x++) {
            float tileX = static_cast<float>(x) * TILE_SIZE;
            float tileY = static_cast<float>(y) * TILE_SIZE;
            if (layer == 0) {
#ifdef ENABLE_LEVEL_EDITOR
            Color tint;
                if (LevelEditor::GetInstance().IsOnionModeEnabled()) {
                    DrawRectangle(tileX, tileY, TILE_SIZE, TILE_SIZE, WHITE);
                    tint = ONION_MODE_TINT;
                } else {
                    tint = WHITE;
                }
                if (GameManager::GetInstance().GetEnviornmentType() == EnviornmentType::OVERWORLD) {
                    DrawTexture(GameAssets::GetInstance().GetTile(BACKGROUND_TILE), tileX, tileY, tint);
                } else {
                    if (IsTileInBounds(x, y, layer)) {
                        DrawTexture(GameAssets::GetInstance().GetTile(FLOOR_TILE), tileX, tileY, tint);
                    } else {
                        DrawRectangle(tileX, tileY, TILE_SIZE, TILE_SIZE, BLACK);
                    }
                }
#else
                if (GameManager::GetInstance().GetEnviornmentType() == EnviornmentType::OVERWORLD) {
                    DrawTexture(GameAssets::GetInstance().GetTile(BACKGROUND_TILE), tileX, tileY, tint);
                } else {
                    if (IsTileInBounds(x, y, layer)) {
                        DrawTexture(GameAssets::GetInstance().GetTile(FLOOR_TILE), tileX, tileY, tint);
                    } else {
                        DrawRectangle(tileX, tileY, TILE_SIZE, TILE_SIZE, BLACK);
                    }
                }
#endif
            }
            short tile = GetTile(x, y, layer);
            if (tile != EMPTY_TILE) {
#ifdef ENABLE_LEVEL_EDITOR
                Color tint = (LevelEditor::GetInstance().IsOnionModeEnabled() && layer != LevelEditor::GetInstance().GetLayer()) ? ONION_MODE_TINT : WHITE;
                if (GameAssets::GetInstance().IsTileAnimates(tile)) {
                    GameAssets::GetInstance().GetAnimation(tile).Draw({tileX, tileY}, static_cast<float>(GetTime()), tint);
                } else {
                    DrawTexture(GameAssets::GetInstance().GetTile(tile), tileX, tileY, tint);
                }
#else
                if (GameAssets::GetInstance().IsTileAnimates(tile)) {
                    GameAssets::GetInstance().GetAnimation(tile).Draw({tileX, tileY}, static_cast<float>(GetTime()));
                } else {
                    DrawTexture(GameAssets::GetInstance().GetTile(tile), tileX, tileY, WHITE);
                }
#endif
            }
        }
    }
}

#ifdef ENABLE_LEVEL_EDITOR
void Tiles::DrawTileData() {
    if (!LevelEditor::GetInstance().IsEditorEnabled()) {
        return;
    }
    for (const auto& [tileIndex, tileData] : tileData) {
            Color tint = (LevelEditor::GetInstance().IsOnionModeEnabled() && LevelEditor::GetInstance().GetLayer() != (tileIndex / GetLayerSize())) ? ONION_MODE_TINT : WHITE;
            int x = tileIndex % width;
            int y = (tileIndex / width) % height;
            float tileX = static_cast<float>(x) * TILE_SIZE;
            float tileY = static_cast<float>(y) * TILE_SIZE;
            constexpr float M_ICON_SIZE_BORDER = M_ICON_SIZE + 4;
            DrawRectangle(tileX - M_ICON_SIZE_BORDER / 2, tileY - M_ICON_SIZE_BORDER / 2, M_ICON_SIZE_BORDER, M_ICON_SIZE_BORDER, BLACK);
            DrawRectangle(tileX - M_ICON_SIZE / 2, tileY - M_ICON_SIZE / 2, M_ICON_SIZE, M_ICON_SIZE, tint);
            constexpr int TEXT_LEFT_ALIGN = 5;
            DrawText("M", tileX - TEXT_LEFT_ALIGN, tileY - TEXT_LEFT_ALIGN, 15, BLACK);
    }
}
#endif

int Tiles::GetLayerSize() {
    return width * height;
}

int Tiles::GetNumTiles() {
    return Tiles::GetLayerSize() * NUM_LAYERS;
}

int Tiles::GetTileDataInt(int tileIndex, int dataIndex) {
    if (dataIndex < 0 || dataIndex >= TILE_DATA_SIZE_INT) {
        throw std::out_of_range("Data index is out of range");
    }
    if (tileData.find(tileIndex) == tileData.end()) {
        return 0;
    }
    return tileData[tileIndex].intData[dataIndex];
}

float Tiles::GetTileDataFloat(int tileIndex, int dataIndex) {
    if (dataIndex < 0 || dataIndex >= TILE_DATA_SIZE_FLOAT) {
        throw std::out_of_range("Data index is out of range");
    }
    if (tileData.find(tileIndex) == tileData.end()) {
        return 0.0f;
    }
    return tileData[tileIndex].floatData[dataIndex];
}

bool Tiles::GetTileDataFlag(int tileIndex, int dataIndex) {
    if (dataIndex < 0 || dataIndex >= 32) {
        throw std::out_of_range("Data index is out of range");
    }
    if (tileData.find(tileIndex) == tileData.end()) {
        return false;
    }
    return (tileData[tileIndex].boolFlags >> dataIndex) & 1;
}

std::string Tiles::GetTileDataString(int tileIndex, int dataIndex) {
    if (dataIndex < 0 || dataIndex >= TILE_DATA_SIZE_STRING) {
        throw std::out_of_range("Data index is out of range");
    }
    if (tileData.find(tileIndex) == tileData.end()) {
        return "";
    }
    return tileData[tileIndex].stringData[dataIndex];
}

void Tiles::SetTileDataInt(int tileIndex, int dataIndex, int value) {
    if (dataIndex < 0 || dataIndex >= TILE_DATA_SIZE_INT) {
        throw std::out_of_range("Data index is out of range");
    }
    tileData[tileIndex].intData[dataIndex] = value;
}

void Tiles::SetTileDataFloat(int tileIndex, int dataIndex, float value) {
    if (dataIndex < 0 || dataIndex >= TILE_DATA_SIZE_FLOAT) {
        throw std::out_of_range("Data index is out of range");
    }
    tileData[tileIndex].floatData[dataIndex] = value;
}

void Tiles::SetTileDataFlag(int tileIndex, int dataIndex, bool value) {
    if (dataIndex < 0 || dataIndex >= 32) {
        throw std::out_of_range("Data index is out of range");
    }
    tileData[tileIndex].boolFlags = (tileData[tileIndex].boolFlags & ~(1 << dataIndex)) | (value << dataIndex);
}

void Tiles::SetTileDataString(int tileIndex, int dataIndex, const std::string& value) {
    if (dataIndex < 0 || dataIndex >= TILE_DATA_SIZE_STRING) {
        throw std::out_of_range("Data index is out of range");
    }
    tileData[tileIndex].stringData[dataIndex] = value;
}

void Tiles::ClearTileData(int tileIndex) {
    tileData.erase(tileIndex);
}

void Tiles::IterateTileData(std::function<void(int tileIndex, TileData data)> callback) {
    for (const auto& [tileIndex, data] : tileData) {
        callback(tileIndex, data);
    }
}

void Tiles::SetBoolFlags(int tileIndex, int boolFlags) {
    tileData[tileIndex].boolFlags = boolFlags;
}

void Tiles::ClearAllTileData() {
    tileData.clear();
}

bool Tiles::IsTileInBounds(int x, int y, int layer) {
    return (x >= 0 && x < width && y >= 0 && y < height && layer >= 0 && layer < NUM_LAYERS);
}