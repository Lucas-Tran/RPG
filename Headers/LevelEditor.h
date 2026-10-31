#ifndef LEVEL_EDITOR_H_INCLUDED
#define LEVEL_EDITOR_H_INCLUDED

#ifdef ENABLE_LEVEL_EDITOR

#include <Engine.h>

class LevelEditor {
public:
    static LevelEditor& GetInstance();

    LevelEditor(const LevelEditor&) = delete;
    LevelEditor& operator=(const LevelEditor&) = delete;

    ~LevelEditor();

    void Update(Camera2D camera);
    void DrawCursor();
    void DrawPallete();
    bool IsEditorEnabled();
    bool IsMouseOffPalette();
    int GetLayer();
    bool IsOnionModeEnabled();
    void TurnEditorOff();
private:
    LevelEditor();
    void SetLayer(int layer);
    void LoadImages();
    void InitializePalleteGrid();
    void LoadLayerMarkingImages();
    void DrawLayerMarkings(short tile, Rectangle box);
    void LoadTileLayers();
    void SaveTileLayers();

    void TurnPinEditorOff();
    void TurnLayerMarkingsEditorOff();

    short* palleteGrid;

    Texture2D outlineTexture;
    Texture2D maskTexture;
    Texture2D layerMarkings[NUM_LAYERS + 1];

    int cursorPositionX;
    int cursorPositionY;

    short selectedPalleteIndex;

    bool editorEnabled;

    float time;

    int palleteScrollX;
    int palleteScrollY;

    int layer;

    bool onionModeEnabled;
    bool autoLayering;

    bool pinEditorEnabled;
    bool tileLayerEditorEnabled;

    char* tileLayers;
};

#endif
#endif