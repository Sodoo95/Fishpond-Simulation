#pragma once
#include <raylib.h>

struct Button {
private:
    Texture2D current;
    Color tint = WHITE;
    float clickTime = -999.0f;
    bool pending = false;

public:
    Vector2 pos;
    Texture2D normal;
    Texture2D hover;
    Texture2D pressed;
    float delay = 0.15f;

    bool isHover() {
        Rectangle area = { pos.x - normal.width / 2, pos.y - normal.height / 2, (float)normal.width, (float)normal.height };
        return CheckCollisionPointRec(GetMousePosition(), area);
    }

    bool isClicked() {
        if (pending && GetTime() - clickTime >= delay) {
            pending = false;
            return true;
        }
        if (isHover() && IsMouseButtonPressed(0)) {
            clickTime = float(GetTime());
            pending = true;
        }
        return false;
    }

    void draw() {
        current = pending ? pressed : (isHover() ? hover : normal);
        tint = pending ? BLACK : (isHover() ? RED : WHITE);
        DrawTexture(current, int(pos.x) - current.width / 2, int(pos.y) - current.height / 2, tint);
    }
};

struct SimpleButton {
private:
    Color current = LIGHTGRAY;
    Color normal = LIGHTGRAY;
    Color hover = GRAY;
    Color pressed = BLACK;
    float clickTime = -999.0f;
    bool pending = false;

public:
    Vector2 pos;
    Vector2 size = { 200, 50 };
    float delay = 0.15f;
    bool isHover() {
        Rectangle area = { pos.x - size.x / 2, pos.y - size.y / 2, size.x, size.y };
        return CheckCollisionPointRec(GetMousePosition(), area);
    }
    bool isClicked() {
        if (pending && GetTime() - clickTime >= delay) {
            pending = false;
            return true;
        }
        if (isHover() && IsMouseButtonPressed(0)) {
            clickTime = float(GetTime());
            pending = true;
        }
        return false;
    }
    void draw(const char* text, int fontSize) {
        current = pending ? pressed : (isHover() ? hover : normal);
        DrawRectangle(int(pos.x - size.x / 2), int(pos.y - size.y / 2), int(size.x), int(size.y), current);
        DrawText(text, int(pos.x - MeasureText(text, fontSize) / 2), int(pos.y - fontSize / 2), fontSize, WHITE);
    }

};