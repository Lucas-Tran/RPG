#include <GameAssets.h>

#include <fstream>

std::string PLAYER_ANIMS_BASE_CLASS = "Top_Down_Adventure_Pack_v.1.0/Char_Sprites/char_";

AnimationAllDirections::AnimationAllDirections(std::string prefixPath, float delay):
leftAnimation(prefixPath + "_left_anim.gif", delay),
rightAnimation(prefixPath + "_right_anim.gif", delay),
upAnimation(prefixPath + "_up_anim.gif", delay),
downAnimation(prefixPath + "_down_anim.gif", delay) {}

Animation& AnimationAllDirections::GetRightAnimation() {
    return rightAnimation;
}

Animation& AnimationAllDirections::GetLeftAnimation() {
    return leftAnimation;
}

Animation& AnimationAllDirections::GetUpAnimation() {
    return upAnimation;
}

Animation& AnimationAllDirections::GetDownAnimation() {
    return downAnimation;
}

constexpr float IdleDelay = 0.15f;
constexpr float RunDelay = 0.1f;
PlayerAnimations::PlayerAnimations():
idleAnimation(PLAYER_ANIMS_BASE_CLASS + "idle", IdleDelay),
runAnimation(PLAYER_ANIMS_BASE_CLASS + "run", RunDelay) {}

AnimationAllDirections& PlayerAnimations::GetIdleAnimation() {
    return idleAnimation;
}

AnimationAllDirections& PlayerAnimations::GetRunAnimation() {
    return runAnimation;
}

GameAssets::GameAssets() {
    LoadTileset("Top_Down_Adventure_Pack_v.1.0/Overworld_Tileset.png", OVERWORLD_TILESET_WIDTH, OVERWORLD_TILESET_HEIGHT);
    LoadTileset("Top_Down_Adventure_Pack_v.1.0/Dungeon_Tileset.png", DUNGEON_TILESET_WIDTH, DUNGEON_TILESET_HEIGHT);
    LoadTilePins(TOTAL_TILES);
}
GameAssets& GameAssets::GetInstance() {
    static GameAssets instance;
    return instance;
}


void GameAssets::LoadTileset(const std::string& path, int tileset_width, int tileset_height) {
    if (tileset_width <= 0 || tileset_height <= 0) {
        throw std::invalid_argument("Tileset dimensions in tiles must be positive");
    }

    Image tileset = LoadImage(path.c_str());
    if (tileset.data == nullptr) {
        throw std::runtime_error("Failed to load tileset image: " + path);
    }

    constexpr int TILE_SIZE_SOURCE = 16;

    tiles.reserve(static_cast<std::size_t>(tileset_width) * tileset_height);

    for (int y = 0; y < tileset.height / TILE_SIZE_SOURCE; y++) {
        for (int x = 0; x < tileset.width / TILE_SIZE_SOURCE; x++) {
            const Rectangle tileBounds = {
                static_cast<float>(x * TILE_SIZE_SOURCE),
                static_cast<float>(y * TILE_SIZE_SOURCE),
                static_cast<float>(TILE_SIZE_SOURCE),
                static_cast<float>(TILE_SIZE_SOURCE)
            };
            Image tileImage = ImageFromImage(tileset, tileBounds);
            ImageResizeNN(&tileImage, TILE_SIZE, TILE_SIZE);
            Texture2D tile = LoadTextureFromImage(tileImage);
            UnloadImage(tileImage);

            if (tile.id == 0) {
                UnloadImage(tileset);
                throw std::runtime_error("Failed to load a tile from tileset: " + path);
            }

            tiles.push_back(tile);
        }
    }

    UnloadImage(tileset);
}

GameAssets::~GameAssets() {
    for (Texture2D& tile : tiles) {
        UnloadTexture(tile);
    }
    tiles.clear();
}

Texture2D GameAssets::GetTile(int index) const {
    if (index < 0 || index >= static_cast<int>(tiles.size())) {
        throw std::out_of_range("Tile index out of range");
    }
    return tiles[index];
}

PlayerAnimations& GameAssets::GetPlayerAnimations() {
    return playerAnimations;
}

constexpr int DATA_PER_PIN = 3;
#ifdef ENABLE_LEVEL_EDITOR
void GameAssets::SetPin(int index, int pinNum, char pinValue) {
    if (index >= 0) {
        int binIndex = pinNum * DATA_PER_PIN;
        tilePins[index] = (tilePins[index] & ~(7 << binIndex)) | (pinValue << binIndex);
    }
}

void GameAssets::SaveTilePins() {
    std::ofstream writer("TilePins.rpgpins");
    int numPins = tilePins.size();
    writer.write(reinterpret_cast<char*>(&numPins), sizeof(int));
    writer.write(reinterpret_cast<char*>(tilePins.data()), sizeof(int) * numPins);
    writer.close();
}
#endif

char GameAssets::GetPin(int index, int pinNum) {
    if (index < 0) {
        return false;
    }
    return (tilePins[index] >> pinNum * DATA_PER_PIN) & 7;
}

void GameAssets::LoadTilePins(int expectedNumPins) {
    std::ifstream reader("TilePins.rpgpins");
    tilePins.reserve(expectedNumPins);

    int numPins = 0;
    if (reader.is_open()) {
        reader.read(reinterpret_cast<char*>(&numPins), sizeof(int));
        for (int i = 0; i < numPins; i++) {
            int pinData;
            reader.read(reinterpret_cast<char*>(&pinData), sizeof(int));
            tilePins.push_back(pinData);
        }
        reader.close();
    }
    if (expectedNumPins > numPins) {
        for (int i = 0; i < expectedNumPins - numPins; i++) {
            tilePins.push_back(0);
        }
    }
}
