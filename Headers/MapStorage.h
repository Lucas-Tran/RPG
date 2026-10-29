#ifndef MAP_STORAGE_H_INCLUDED
#define MAP_STORAGE_H_INCLUDED

#include <Engine.h>

void SetSceneName(std::string newName);

std::string GetSceneName();

void NewMap(int width, int height);

void SaveScene();

void LoadScene();

#endif