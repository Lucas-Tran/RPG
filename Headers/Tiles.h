#ifndef TILES_H_INCLUDED
#define TILES_H_INCLUDED

class Tiles {
public:
    Tiles(const Tiles&) = delete;
    Tiles& operator=(const Tiles&) = delete;
    ~Tiles();

    static Tiles& GetInstance();

    void Init(int width, int height);
    short GetTile(int index) const;
    void SetTile(int index, short tile);

    void Draw();

private:
    Tiles();

    short* map = nullptr;
    int width = 0;
    int height = 0;
};

#endif