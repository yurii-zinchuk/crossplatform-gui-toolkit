//
// Created by yuriizin on 22.11.23.
//

#include <string>

class MyText {
public:
    MyText(std::string text, int color, int x, int y) :
    text(std::move(text)), x(x), y(y), color(color) {

    }

    std::string text;
    int x{}, y{}, color{};
};
