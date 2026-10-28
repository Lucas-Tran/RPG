#include <Tiles.h>

#include <Engine.h>
#include <GameAssets.h>

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

    short* newMap = new short[mapWidth * mapHeight]{};
    delete[] map;
    map = newMap;
    this->width = width;
    this->height = height;
}

short Tiles::GetTile(int index) const {
    if (map == nullptr) {
        throw std::logic_error("Map has not been initialized");
    }
    if (index < 0 || static_cast<std::size_t>(index) >=
                         static_cast<std::size_t>(width) * height) {
        throw std::out_of_range("Tile index is outside the map");
    }
    return map[index];
}

void Tiles::SetTile(int index, short tile) {
    if (map == nullptr) {
        throw std::logic_error("Map has not been initialized");
    }
    if (index < 0 || static_cast<std::size_t>(index) >=
                         static_cast<std::size_t>(width) * height) {
        throw std::out_of_range("Tile index is outside the map");
    }
    map[index] = tile;
}

int Tiles::GetWidth() const {
    return width;
}

int Tiles::GetHeight() const {
    return height;
}

constexpr short BACKGROUND_TILE = 24;

void Tiles::DrawVisibleRegion(Camera2D& camera2D) const {
    int screenWidthTiles = GetScreenWidth() / TILE_SIZE + 2;
    int screenHeightTiles = GetScreenHeight() / TILE_SIZE + 2;
    int drawOffsetX = -static_cast<int>(camera2D.offset.x);
    int drawOffsetY = -static_cast<int>(camera2D.offset.y);
    int gridX = static_cast<int>(floor((camera2D.target.x + drawOffsetX) / TILE_SIZE));
    int gridY = static_cast<int>(floor((camera2D.target.y + drawOffsetY) / TILE_SIZE));
    for (int y = gridY; y < gridY + screenHeightTiles; y++) {
        for (int x = gridX; x < gridX + screenWidthTiles; x++) {
            int gridIndex = y * width + x;
            float tileX = static_cast<float>(x) * TILE_SIZE;
            float tileY = static_cast<float>(y) * TILE_SIZE;
                DrawTexture(GameAssets::GetInstance().GetTile(BACKGROUND_TILE), tileX, tileY, WHITE);
            if (gridIndex >= 0 && gridIndex < width * height && GetTile(gridIndex) != EMPTY_TILE) {
                DrawTexture(GameAssets::GetInstance().GetTile(GetTile(gridIndex)), tileX, tileY, WHITE);
            }
        }
    }
}
