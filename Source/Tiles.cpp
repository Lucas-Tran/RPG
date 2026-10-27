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

void Tiles::Draw() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int gidx = y * width + x;
            DrawTexture(GameAssets::GetInstance().GetTile(map[gidx]), x * TILE_SIZE, y * TILE_SIZE, WHITE);
        }
    }
}
