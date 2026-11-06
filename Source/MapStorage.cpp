#include <MapStorage.h>


#include <Tiles.h>

#include <fstream>

#include <stdexcept>

#include <GameManager.h>


constexpr int BUSH_TILE = 59;

std::string sceneName;

void SetSceneName(std::string newName) {
    sceneName = newName;
}

std::string GetSceneName() {
    return sceneName;
}

void NewMapOver(int width, int height) {
    Tiles& tiles = Tiles::GetInstance();
    tiles.ClearAllTileData();
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


void NewMapBuilding(int width, int height) {
    Tiles& tiles = Tiles::GetInstance();
    tiles.ClearAllTileData();
    tiles.Init(width, height);
    int i = 0;

    // Wall row #1
    tiles.SetTile(i, 104);
    i++;
    for (int j = 0; j < tiles.GetWidth() - 2; j++) {
        tiles.SetTile(i, 105);
        i++;
    }
    tiles.SetTile(i, 107);
    i++;

    // Wall row #2
    tiles.SetTile(i, 122);
    i++;
    for (int j = 0; j < tiles.GetWidth() - 2; j++) {
        tiles.SetTile(i, 123);
        i++;
    }
    tiles.SetTile(i, 125);
    i++;

    // Wall row #3
    tiles.SetTile(i, 140);
    i++;
    for (int j = 0; j < tiles.GetWidth() - 2; j++) {
        tiles.SetTile(i, 141);
        i++;
    }
    tiles.SetTile(i, 143);
    i++;

    // Vertical walls
    for (int j = 0; j < tiles.GetHeight() - 4; j++) {
        tiles.SetTile(i, 158);
        i++;
        for (int k = 0; k < tiles.GetWidth() - 2; k++) {
            tiles.SetTile(i, 159);
            i++;
        }
        tiles.SetTile(i, 161);
        i++;
    }

    for (int j = 0; j < tiles.GetNumTiles() - tiles.GetLayerSize(); j++) {
        tiles.SetTile(i, EMPTY_TILE);
        i++;
    }

    // Bottom walls
    tiles.SetTile(i, 176);
    i++;
    tiles.SetTile(i, 177);
    i++;
    for (int j = 0; j < tiles.GetWidth() - 3; j++) {
        tiles.SetTile(i, 178);
        i++;
    }
    tiles.SetTile(i, 179);
    i++;
}

constexpr char DELIM = '|';

void WriteValue(std::ofstream& writer, std::string value) {
    writer << value << DELIM;
}

void WriteValue(std::ofstream& writer, int value) {
    writer << value << DELIM;
}

void WriteValue(std::ofstream& writer, float value) {
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

void SaveTileData(std::ofstream& writer) {
    WriteValue(writer, "TILEDATA.1");
    auto writerLambda = [&writer](int tileIndex, TileData data) {
        WriteValue(writer, tileIndex);
        for (int i = 0; i < TILE_DATA_SIZE_INT; i++) {
            WriteValue(writer, data.intData[i]);
        }
        for (int i = 0; i < TILE_DATA_SIZE_FLOAT; i++) {
            WriteValue(writer, data.floatData[i]);
        }
        for (int i = 0; i < TILE_DATA_SIZE_STRING; i++) {
            WriteValue(writer, data.stringData[i]);
        }
        WriteValue(writer, data.boolFlags);
    };
    Tiles::GetInstance().IterateTileData(writerLambda);
    WriteValue(writer, "END");
}

void SaveScene() {
    std::ofstream writer("Scenes/" + sceneName + ".rpgscene");
    WriteValue(writer, "RPG");

    SaveTilemap(writer);
    SaveTileData(writer);
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

float ReadValueF(std::ifstream& reader) {
    return stof(ReadValueS(reader));
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

void LoadTileData(std::ifstream& reader) {
    Tiles::GetInstance().ClearAllTileData();
    std::string next = ReadValueS(reader);
    while (next != "END") {
        int tileIndex = std::stoi(next);
        for (int i = 0; i < TILE_DATA_SIZE_INT; i++) {
            Tiles::GetInstance().SetTileDataInt(tileIndex, i, ReadValueI(reader));
        }
        for (int i = 0; i < TILE_DATA_SIZE_FLOAT; i++) {
            Tiles::GetInstance().SetTileDataFloat(tileIndex, i, ReadValueF(reader));
        }
        for (int i = 0; i < TILE_DATA_SIZE_STRING; i++) {
            Tiles::GetInstance().SetTileDataString(tileIndex, i, ReadValueS(reader));
        }
        Tiles::GetInstance().SetBoolFlags(tileIndex, ReadValueI(reader));
        next = ReadValueS(reader);
    }
}

void LoadScene() {
    std::ifstream reader("Scenes/" + sceneName + ".rpgscene");
    if (sceneName.contains("over")) {
        GameManager::GetInstance().SetEnviornmentType(EnviornmentType::OVERWORLD);
    } else if (sceneName.contains("building")) {
        GameManager::GetInstance().SetEnviornmentType(EnviornmentType::BUILDING);
    } else {
        throw std::runtime_error("Unknown scene type: " + sceneName);
    }
    if (!reader.is_open()) {
        switch (GameManager::GetInstance().GetEnviornmentType()) {
            case EnviornmentType::OVERWORLD:
                NewMapOver(DEFAULT_MAP_SIZE_OVER, DEFAULT_MAP_SIZE_OVER);
                break;
            case EnviornmentType::BUILDING:
                int buildingWidth, buildingHeight;
                std::cout << "Enter building width: ";
                std::cin >> buildingWidth;
                std::cout << "Enter building height: ";
                std::cin >> buildingHeight;
                NewMapBuilding(buildingWidth, buildingHeight);
                break;
        }
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
        } else if (module.contains("TILEDATA.")) {
            LoadTileData(reader);
        } else {
            throw std::runtime_error("Unexpected Module: " + module);
        }
    }
    reader.close();
}
