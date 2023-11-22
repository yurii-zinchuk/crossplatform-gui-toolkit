//
// Created by yuriizin on 22.11.23.
//

#include "main.cpp"

int main() {
    MyGUI gui;

    MyButton button("Click me!", 0x0000FF, 10, 10, 100, 30);
    MyText text("Hello, world!", 0x000000, 10, 50);

    button.onClick = [&gui]() {
        gui.addText(
                MyText("Button clicked!", 0x000000, 100, 50)
        );
    };

    gui.addButton(button);
    gui.addText(text);

    gui.Run();
}
