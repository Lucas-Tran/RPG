#include <GameManager.h>

GameManager::GameManager() {
    cutsceneFunction = nullptr;
}

void GameManager::SetCutsceneFunction(std::function<void()> cutsceneFunction) {
    this->cutsceneFunction = cutsceneFunction;
}

void GameManager::CheckAndDoCutscenes() {
    if (cutsceneFunction != nullptr) {
        cutsceneFunction();
        cutsceneFunction = nullptr;
    }
}
