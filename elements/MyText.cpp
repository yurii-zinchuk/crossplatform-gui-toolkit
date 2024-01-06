//
// Created by yuriizin on 22.11.23.
//

#include <string>

class MyText {
public:
    MyText(std::string text, int color, int n_color, int x, int y) :
            text(std::move(text)), x(x), y(y), color(color), n_color(n_color), inputId(-1) {}

    MyText(int inputId, int color, int n_color, int x, int y) :
            inputId(inputId), x(x), y(y), color(color), n_color(n_color) {}

    std::string text;
    int x{}, y{}, color{}, n_color{};
    int inputId{};
};
