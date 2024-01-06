//
// Created by yuriizin on 22.11.23.
//

#include <string>
#include <functional>

class MyButton {
public:
    MyButton(std::string text, int color, int n_color, int text_color, int text_n_color, int x, int y, int width, int height) :
            text(std::move(text)), x(x), y(y), width(width), height(height), color(color), n_color(n_color), text_color(text_color), text_n_color(text_n_color) {}

    MyButton() = default;

    std::string text;
    int x{}, y{}, width{}, height{}, color{}, n_color{}, text_color{}, text_n_color{};

    std::function<void()> onClick = []() {};

    bool isClicked(int mouseX, int mouseY) const {
        return (mouseX >= x && mouseX <= (x + width) && mouseY >= y && mouseY <= (y + height));
    }
};
