#include <MapStorage.h>

#include <Engine.h>

constexpr int BUSH_TILE = 59;

void NewMap(Tiles& tiles, int width, int height) {
    tiles.Init(width, height);

    int i = 0;
    for (int j = 0; j < width; j++) {
        tiles.SetTile(i, BUSH_TILE);
        i++;
    }
    for (int j = 0; j < height - 2; j++) {
        tiles.SetTile(i, BUSH_TILE);
        i++;
        for (int k = 0; k < width - 2; k++) {
            tiles.SetTile(i, EMPTY_TILE);
            i++;
        }
        tiles.SetTile(i, BUSH_TILE);
        i++;
    }
    for (int j = 0; j < width; j++) {
        tiles.SetTile(i, BUSH_TILE);
        i++;
    }

}