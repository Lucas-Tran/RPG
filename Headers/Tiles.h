#ifndef TILES_H_INCLUDED
#define TILES_H_INCLUDED

#include "Engine.h"

class Tiles {
public:
    Tiles(const Tiles&) = delete;
    Tiles& operator=(const Tiles&) = delete;
    ~Tiles();

    static Tiles& GetInstance();

    void Init(int width, int height);
    short GetTile(int index) const;
    void SetTile(int index, short tile);
    short GetTile(int x, int y, int layer) const;
    void SetTile(int x, int y, int layer, short tile);
    int GetLayerSize() const;

    int GetWidth() const;
    int GetHeight() const;

    void DrawVisibleRegion(Camera2D& camera2D, int layer) const;

private:
    Tiles();

    short* map = nullptr;
    int width;
    int height;
};

#endif