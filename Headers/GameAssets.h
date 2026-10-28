#ifndef GAME_ASSETS_H_INCLUDED
#define GAME_ASSETS_H_INCLUDED

#include <Engine.h>

#include <Animation.h>

class AnimationAllDirections {
public:
    AnimationAllDirections(std::string prefixPath, float delay);

    Animation& GetRightAnimation();
    Animation& GetLeftAnimation();
    Animation& GetUpAnimation();
    Animation& GetDownAnimation();
private:
    Animation rightAnimation;
    Animation leftAnimation;
    Animation upAnimation;
    Animation downAnimation;
};

class PlayerAnimations {
public:
    PlayerAnimations();

    AnimationAllDirections& GetIdleAnimation();
    AnimationAllDirections& GetRunAnimation();
private:
    AnimationAllDirections idleAnimation;
    AnimationAllDirections runAnimation;
};

class GameAssets {
public:
    static GameAssets& GetInstance();


    GameAssets(const GameAssets&) = delete;
    GameAssets& operator=(const GameAssets&) = delete;


    void LoadTileset(const std::string& path, int tileset_width, int tileset_height);
    void UnloadTiles();

    Texture2D GetTile(int index) const;

    PlayerAnimations& GetPlayerAnimations();
private:
    GameAssets();

    std::vector<Texture2D> tiles;

    PlayerAnimations playerAnimations;
};

#endif