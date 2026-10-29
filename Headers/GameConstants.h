#ifndef GAME_CONSTANTS_H_INCLUDED
#define GAME_CONSTANTS_H_INCLUDED

constexpr int TILESET_WIDTH = 18;
constexpr int TILESET_HEIGHT = 13;
constexpr int TILESET_SIZE = TILESET_WIDTH * TILESET_HEIGHT;

constexpr float PLAYER_SPEED = 160.0f;
constexpr int TILE_SIZE = 64;
constexpr int EMPTY_TILE = -1;

constexpr int PADDING = 8;
constexpr int PALETTE_WIDTH = 5;
constexpr int PALETTE_HEIGHT = 11;
constexpr int PALETTE_WIDTH_PIXELS = PALETTE_WIDTH * TILE_SIZE + 2 * PADDING;

constexpr int NUM_LAYERS = 3;


#endif