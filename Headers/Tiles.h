#ifndef TILES_H_INCLUDED
#define TILES_H_INCLUDED

#include "Engine.h"

#include <array>
#include <string>
#include <functional>

struct TileData {
    std::array<int, TILE_DATA_SIZE_INT> intData = {};
    std::array<float, TILE_DATA_SIZE_FLOAT> floatData = {};
    std::array<bool, TILE_DATA_SIZE_BOOL> boolData = {};
    std::array<std::string, TILE_DATA_SIZE_STRING> stringData;
};

class Tiles {
public:
    Tiles(const Tiles&) = delete;
    Tiles& operator=(const Tiles&) = delete;
    ~Tiles();

    static Tiles& GetInstance();

    void Init(int width, int height);
    short GetTile(int index);
    void SetTile(int index, short tile);
    short GetTile(int x, int y, int layer);
    void SetTile(int x, int y, int layer, short tile);
    int GetLayerSize();
    int GetNumTiles();

    int GetWidth();
    int GetHeight();

    void DrawVisibleRegion(Camera2D& camera2D, int layer);
#ifdef ENABLE_LEVEL_EDITOR
    void DrawTileData();
#endif

    int GetTileDataInt(int tileIndex, int dataIndex);
    float GetTileDataFloat(int tileIndex, int dataIndex);
    bool GetTileDataBool(int tileIndex, int dataIndex);
    std::string GetTileDataString(int tileIndex, int dataIndex);

    void SetTileDataInt(int tileIndex, int dataIndex, int value);
    void SetTileDataFloat(int tileIndex, int dataIndex, float value);
    void SetTileDataBool(int tileIndex, int dataIndex, bool value);
    void SetTileDataString(int tileIndex, int dataIndex, const std::string& value);

    void ClearTileData(int tileIndex);

    void IterateTileData(std::function<void(int tileIndex, TileData data)> callback);
private:
    Tiles();

    short* map = nullptr;
    int width;
    int height;

    std::unordered_map<int, TileData> tileData;
};

#endif