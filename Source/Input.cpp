#include <Input.h>

Input::Input() = default;

Input& Input::GetInstance() {
    static Input instance;
    return instance;
}

void Input::Update() {
    bool keyUp = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    bool keyDown = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
    bool keyLeft = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    bool keyRight = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
    joystickX = static_cast<float>(keyRight) - static_cast<float>(keyLeft);
    joystickY = static_cast<float>(keyDown) - static_cast<float>(keyUp);

    joyStickNormalized = Vector2{ joystickX, joystickY };

    joyStickNormalized = Vector2Normalize(joyStickNormalized);

}

int Input::GetJoystickX() {
    return joystickX;
}

int Input::GetJoystickY() {
    return joystickY;
}

Vector2 Input::GetInputNormalized() {
    return joyStickNormalized;
}
