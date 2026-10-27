#ifndef GAME_ASSETS_H_INCLUDED
#define GAME_ASSETS_H_INCLUDED

#include <Engine.h>

class GameAssets {
public:
    static GameAssets& GetInstance();


    GameAssets(const GameAssets&) = delete;
    GameAssets& operator=(const GameAssets&) = delete;


    void LoadTileset(const std::string& path, int tileset_width, int tileset_height);
    void UnloadTiles();

    Texture2D GetTile(int index) const;
private:
    std::vector<Texture2D> tiles;
    GameAssets();
};

#endif