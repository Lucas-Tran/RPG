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

    int GetWidth() const;
    int GetHeight() const;

    void DrawVisibleRegion(Camera2D& camera2D) const;

private:
    Tiles();

    short* map = nullptr;
    int width = 0;
    int height = 0;
};

#endif