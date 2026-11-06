#ifndef GAME_MANAGER_H_INCLUDED
#define GAME_MANAGER_H_INCLUDED

#include <functional>

enum class EnviornmentType {
    OVERWORLD,
    BUILDING
};

class GameManager {
public:
    GameManager();
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    static GameManager& GetInstance() {
        static GameManager instance;
        return instance;
    }

    void SetCutsceneFunction(std::function<void()> cutsceneFunction);

    void CheckAndDoCutscenes();

    void SetEnviornmentType(EnviornmentType type);

    EnviornmentType GetEnviornmentType();
private:

    EnviornmentType enviornmentType;

    std::function<void()> cutsceneFunction;
};

#endif