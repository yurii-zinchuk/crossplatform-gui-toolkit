//
// Created by yuriizin on 26.11.23.
//

#include <string>
#include <functional>
#include <utility>

class MyTextInput {
public:
    std::string text;
    int color;
    int x, y, width, height;

    std::function<void()> onClick = []() {

    };

    std::string text_ref = &text;

    MyTextInput(int color, int x, int y, int width, int height) {
        this->color = color;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }

    bool isClicked(int mouseX, int mouseY) const {
        return (mouseX >= x && mouseX <= (x + width) && mouseY >= y && mouseY <= (y + height));
    }
};