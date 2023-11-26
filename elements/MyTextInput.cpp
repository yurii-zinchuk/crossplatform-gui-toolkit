//
// Created by yuriizin on 26.11.23.
//

#include <string>

class MyTextInput {
    int idCounter = 0;

public:
    std::string text;
    int color;
    int x, y, width, height;
    int id = idCounter++;

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
