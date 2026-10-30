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
    ~GameAssets();

    Texture2D GetTile(int index) const;

    PlayerAnimations& GetPlayerAnimations();
#ifdef ENABLE_LEVEL_EDITOR
    void SetPin(int index, int pinNum, char pinValue);
#endif
    char GetPin(int index, int pinNum);

#ifdef ENABLE_LEVEL_EDITOR
    void SaveTilePins();
#endif
private:
    GameAssets();

    void LoadTilePins(int expectedNumPins);

    std::vector<Texture2D> tiles;

    std::vector<int> tilePins;

    PlayerAnimations playerAnimations;
};

#endif