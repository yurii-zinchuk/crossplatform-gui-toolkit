//
// Created by yuriizin on 22.11.23.
//


#include <string>
#include <functional>

class MyButton {
public:
    MyButton(std::string text, int color, int x, int y, int width, int height) : text(std::move(text)), x(x), y(y),
                                                                                 width(width),
                                                                                 height(height), color(color) {
    }

    MyButton() = default;

    std::string text;
    int x{}, y{}, width{}, height{}, color{};

    std::function<void()> onClick = []() {};

    bool isClicked(int mouseX, int mouseY) const {
        return (mouseX >= x && mouseX <= (x + width) && mouseY >= y && mouseY <= (y + height));
    }
};
