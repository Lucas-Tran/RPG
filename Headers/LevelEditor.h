#ifndef LEVEL_EDITOR_H_INCLUDED
#define LEVEL_EDITOR_H_INCLUDED

#ifdef ENABLE_LEVEL_EDITOR

#include <Engine.h>

class LevelEditor {
public:
    static LevelEditor& GetInstance();

    LevelEditor(const LevelEditor&) = delete;
    LevelEditor& operator=(const LevelEditor&) = delete;

    void Update(Camera2D camera);

    void DrawCursor();
    void DrawPallete();

    bool IsEditorEnabled();

    bool IsMouseOffPalette();

    int GetLayer();
    bool IsOnionModeEnabled();
private:
    LevelEditor();

    void SetLayer(int layer);

    Texture2D outlineTexture;
    Texture2D maskTexture;

    int cursorPositionX;
    int cursorPositionY;

    short selectedTile;

    bool editorEnabled;

    float time;

    int palleteScrollX;
    int palleteScrollY;

    int layer;

    bool onionModeEnabled;
};

#endif
#endif