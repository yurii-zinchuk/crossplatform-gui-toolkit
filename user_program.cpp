//
// Created by yuriizin on 22.11.23.
//

#include "main.cpp"

int main() {
    MyGUI gui;

    MyButton button("Click me!", 0x0000FF, 10, 10, 100, 30);
    MyButton button2("Click me 2!", 0x0000FF, 210, 10, 100, 30);
    MyText text("Hello, world!", 0x000000, 10, 50);
    MyTextInput input(0x000000, 10, 100, 100, 30);

    button.onClick = [&gui]() {
        gui.addText(
                MyText("Button clicked!", 0x000000, 100, 50)
        );
    };



    button2.onClick = [&gui, &input]() {
//        gui.addText(
//                MyText("Button clicked 2!", 0x000000, 200, 50)
//        );



        gui.addText(
                MyText(input.text, 0x000000, 200, 50)
        );
    };

    gui.addButton(button);
    gui.addButton(button2);
    gui.addTextInput(input);

    gui.addText(text);

    gui.Run();
}
