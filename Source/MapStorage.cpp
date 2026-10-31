#include <MapStorage.h>


#include <Tiles.h>

#include <fstream>

#include <stdexcept>


constexpr int BUSH_TILE = 59;

std::string sceneName;

void SetSceneName(std::string newName) {
    sceneName = newName;
}

std::string GetSceneName() {
    return sceneName;
}

void NewMap(int width, int height) {
    Tiles& tiles = Tiles::GetInstance();
    tiles.Init(width, height);

    int i = 0;
    for (int j = 0; j < width; j++) {
        tiles.SetTile(i, BUSH_TILE);
        i++;
    }
    for (int j = 0; j < height - 2; j++) {
        tiles.SetTile(i, BUSH_TILE);
        i++;
        for (int k = 0; k < width - 2; k++) {
            if (GetRandomValue(1, 10) == 1) {
                tiles.SetTile(i, GetRandomValue(BUSH_TILE - 3, BUSH_TILE));
            } else {
                tiles.SetTile(i, EMPTY_TILE);
            }
            i++;
        }
        tiles.SetTile(i, BUSH_TILE);
        i++;
    }
    for (int j = 0; j < width; j++) {
        tiles.SetTile(i, BUSH_TILE);
        i++;
    }
    for (int j = 0; j < Tiles::GetInstance().GetLayerSize() * (NUM_LAYERS - 1); j++) {
        tiles.SetTile(i, EMPTY_TILE);
        i++;
    }

}

constexpr char DELIM = '|';

void WriteValue(std::ofstream& writer, std::string value) {
    writer << value << DELIM;
}

void WriteValue(std::ofstream& writer, int value) {
    writer << value << DELIM;
}

void SaveTilemap(std::ofstream& writer) {
    WriteValue(writer, "TILEMAP.1");
    WriteValue(writer, Tiles::GetInstance().GetWidth());
    WriteValue(writer, Tiles::GetInstance().GetHeight());
    int i = 0;
    while (i < Tiles::GetInstance().GetNumTiles()) {
        int count = 1;
        short tile = Tiles::GetInstance().GetTile(i);
        i++;
        while (i < Tiles::GetInstance().GetNumTiles() && Tiles::GetInstance().GetTile(i) == tile) {
            i++;
            count++;
        }
        WriteValue(writer, count);
        WriteValue(writer, tile);
    }
    WriteValue(writer, "END");
}

void SaveScene() {
    std::ofstream writer("Scenes/" + sceneName + ".rpgscene");
    WriteValue(writer, "RPG");

    SaveTilemap(writer);
    writer.close();
}

std::string ReadValueS(std::ifstream& reader) {
    std::string value;
    std::getline(reader, value, DELIM);
    return value;
}

int ReadValueI(std::ifstream& reader) {
    return stoi(ReadValueS(reader));
}

void LoadTilemap(std::ifstream& reader) {
    Tiles::GetInstance().Init(ReadValueI(reader), ReadValueI(reader));
    std::string next = ReadValueS(reader);
    int i = 0;
    while (next != "END") {
        int count = std::stoi(next);
        short tile = ReadValueI(reader);
        for (int j = 0; j < count; j++) {
            Tiles::GetInstance().SetTile(i, tile);
            i++;
        }
        next = ReadValueS(reader);
    }
}

void LoadScene() {
    std::ifstream reader("Scenes/" + sceneName + ".rpgscene");
    if (!reader.is_open()) {
        NewMap(DEFAULT_MAP_SIZE, DEFAULT_MAP_SIZE);
        return;
    }
    if (ReadValueS(reader) != "RPG") {
        throw std::runtime_error("Expected RPG!");
    }
    while (true) {
        std::string module = ReadValueS(reader);

        if (!reader) {
            return;
        }

        if (module.empty()) {
            continue;
        }


        if (module.contains("TILEMAP.")) {
            LoadTilemap(reader);
        } else {
            throw std::runtime_error("Unexpected Module: " + module);
        }
    }
    reader.close();
}
