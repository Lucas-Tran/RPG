#ifndef GAME_MANAGER_H_INCLUDED
#define GAME_MANAGER_H_INCLUDED

#include <functional>

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
private:

    std::function<void()> cutsceneFunction;
};

#endif