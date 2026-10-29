#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include <Engine.h>

class Input {
public:
    static Input& GetInstance();

    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    void Update();

    int GetJoystickX();

    int GetJoystickY();

    Vector2 GetInputNormalized();
private:
    Input();

    float joystickX;
    float joystickY;

    Vector2 joyStickNormalized;
};

#endif