#include "main.cpp"
#include <iostream>

int main() {
    MyGUI gui(0xffffff, 0x101011, 1280, 720);

    int centerX = 1280/2;
    int centerY = 720 / 2;

    MyText hello_text("Hello! Thank you for choosing to fill out our questionnaire from the company POC/ACS/OS!",
                      0x453f3d, 0xD3D3D3, centerX - 300, centerY - 250);
    MyText name_text("Enter Your Name:", 0x333232, 0xD3D3D3, centerX - 300, centerY - 200);

    MyTextInput name_input(0xB7B7B7, 0xD6D6D6, 0x000000, 0x514B23, centerX - 100, centerY - 220, 200, 30, 0);

    MyText surname_text("Enter Your Surname:", 0x333232, 0xD3D3D3, centerX - 300, centerY - 150);
    MyTextInput surname_input(0xB7B7B7, 0xD6D6D6, 0x000000, 0x514B23, centerX - 100, centerY - 170, 16, 30, 1);

    MyText age_text("Enter Your Birth year:", 0x333232, 0xD3D3D3, centerX - 300, centerY - 100);
    MyTextInput birth_date_input(0xB7B7B7, 0xD6D6D6, 0x000000, 0x514B23, centerX - 100, centerY - 120, 200, 4, 2);

    MyButton submit_button("Submit", 0x453f3d, 0xD6D6D6, 0xffffff, 0x514B23, centerX - 100, centerY - 20, 15, 4, 3);

    submit_button.onClick = [&gui, &name_input, &surname_input, &birth_date_input, centerX, centerY]() {
        gui.addText(
                MyText("Your name is ", 0x333232, 0xD3D3D3, centerX - 150, centerY + 50)
        );
        gui.addText(
                MyText(name_input.id, 0x000000, 0xD3D3D3, centerX - 20, centerY + 50)
        );
        gui.addText(
                MyText("Your surname is ", 0x333232, 0xD3D3D3, centerX - 150, centerY + 100)
        );
        gui.addText(
                MyText(surname_input.id, 0x000000, 0xD3D3D3, centerX - 20, centerY + 100)
        );
        gui.addText(
                MyText("You were born on ", 0x333232, 0xD3D3D3, centerX - 150, centerY + 150)
        );
        gui.addText(
                MyText(birth_date_input.id, 0x000000, 0xD3D3D3, centerX - 20, centerY + 150)
        );
        gui.addText(
                MyText("Your age is ", 0x333232, 0xD3D3D3, centerX - 150, centerY + 200)
        );

        int age = 2024 - std::stoi(gui.return_text_input(birth_date_input.id));
        gui.addText(
                MyText(std::to_string(age), 0x333232, 0xD3D3D3, centerX - 20, centerY + 200)
        );
        gui.addText(
                MyText("Thank you for filling out our questionnaire!", 0x333232, 0xD3D3D3, centerX - 100, centerY + 250)
        );

    };

    gui.addButton(submit_button);
    gui.addText(hello_text);
    gui.addText(name_text);
    gui.addTextInput(name_input);
    gui.addText(surname_text);
    gui.addTextInput(surname_input);
    gui.addTextInput(birth_date_input);
    gui.addText(age_text);

    gui.Run();
}
