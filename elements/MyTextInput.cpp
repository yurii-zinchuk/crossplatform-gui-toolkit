//
// Created by yuriizin on 26.11.23.
//

#include <string>

class MyTextInput {

public:
    static inline int idCounter = 0;
    std::string text;
    int color;
    int n_color;
    int text_color;
    int text_n_color;
    int x, y, width, height;
    int relative_size;
    int id;

    MyTextInput(int color, int n_color, int text_color, int text_n_color, int x, int y, int width, int height, int relative_size = 0) {
        this->color = color;
        this->n_color = n_color;
        this->text_color = text_color;
        this->text_n_color = text_n_color;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->relative_size = relative_size;
        id = idCounter++;
    }

    bool isClicked(int mouseX, int mouseY) const {
        return (mouseX >= x && mouseX <= (x + width) && mouseY >= y && mouseY <= (y + height));
    }
};
