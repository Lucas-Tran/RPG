#ifndef GAME_ASSETS_H_INCLUDED
#define GAME_ASSETS_H_INCLUDED

#include <Engine.h>

#include <Animation.h>

#include <map>

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

    Animation& GetClimbUpAnimation();
    Animation& GetClimbDownAnimation();
private:
    AnimationAllDirections idleAnimation;
    AnimationAllDirections runAnimation;
    Animation climbUpAnimation;
    Animation climbDownAnimation;
};

class GameAssets {
public:
    static GameAssets& GetInstance();


    GameAssets(const GameAssets&) = delete;
    GameAssets& operator=(const GameAssets&) = delete;


    void LoadTileset(const std::string& path, int tileset_width, int tileset_height);
    ~GameAssets();

    Texture2D GetTile(short tile) const;

    PlayerAnimations& GetPlayerAnimations();
#ifdef ENABLE_LEVEL_EDITOR
    void SetPin(short tile, int pinNum, char pinValue);
#endif
    char GetPin(short tile, int pinNum);

    bool IsTileAnimates(short index);
#ifdef ENABLE_LEVEL_EDITOR
    void SaveTilePins();
#endif
    Animation& GetAnimation(short tile);
private:
    GameAssets();

    void LoadTilePins(short expectedNumPins);
    void MarkAnimateFlag(short index);
    void LoadAnimatedTile(std::string path, float frameDelay, int width, int height);

    void LoadAnimatedTileDirectory(std::string directory);

    std::vector<Texture2D> tiles;
    std::unordered_map<short, Animation> tileAnimations;

    std::vector<int> tilePins;

    PlayerAnimations playerAnimations;
};

#endif