//
// Created by yuriizin on 22.11.23.
//

#include "main.cpp"
#include <iostream>

int main() {
    MyGUI gui(0xFFFFFF, 0x3d3d3d, 800, 600);

    MyText hello_text("Hello! Thank you for choosing to fill out our questionnaire from the company POC/ACS/OS!",
                      0x453f3d, 0xffffff, 100, 50);
    MyText name_text("Enter Your Name:", 0x333232, 0x000000, 100, 100);

    MyTextInput name_input(0xB7B7B7, 0x000000, 0x000000, 0xffffff, 250, 80, 200, 30);


    MyText surname_text("Enter Your Surname:", 0x333232, 0x000000, 100, 150);
    MyTextInput surname_input(0xB7B7B7, 0x000000, 0x000000, 0xffffff, 250, 130, 200, 30);

    MyText age_text("Enter Your Birth year:", 0x333232, 0x00000, 100, 200);
    MyTextInput birth_date_input(0xB7B7B7, 0x000000, 0x000000, 0xffffff, 250, 180, 200, 30);

    MyButton submit_button("Submit", 0x453f3d, 0x000000, 0x000000, 0xffffff, 250, 230, 200, 30, 0);


    submit_button.onClick = [&gui, &name_input, &surname_input, &birth_date_input]() {
        gui.addText(
                MyText("Your name is ", 0x333232, 0x000000, 100, 300)
        );
        gui.addText(
                MyText(name_input.id, 0x000000, 0xffffff, 220, 300)
        );
        gui.addText(
                MyText("Your surname is ", 0x333232, 0xffffff, 100, 350)
        );
        gui.addText(
                MyText(surname_input.id, 0x000000, 0xffffff, 220, 350)
        );
        gui.addText(
                MyText("You were born on ", 0x333232, 0xffffff, 100, 400)
        );
        gui.addText(
                MyText(birth_date_input.id, 0x000000, 0xffffff, 220, 400)
        );
        gui.addText(
                MyText("Your age is ", 0x333232, 0xffffff, 100, 450)
        );

        int age = 2023 - std::stoi(gui.return_text_input(birth_date_input.id));
        gui.addText(
                MyText(std::to_string(age), 0x333232, 0xffffff, 220, 450)
        );
        gui.addText(
                MyText("Thank you for filling out our questionnaire!", 0x333232, 0xffffff, 200, 500)
        );

    };


    gui.addButton(submit_button);
    gui.addText(age_text);
    gui.addTextInput(birth_date_input);
    gui.addText(surname_text);
    gui.addTextInput(surname_input);
    gui.addTextInput(name_input);

    gui.addText(hello_text);
    gui.addText(name_text);

    gui.Run();
}
