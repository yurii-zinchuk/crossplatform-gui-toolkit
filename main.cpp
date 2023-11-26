//
// Created by yuriizin on 22.11.23.
//

#include <cstdio>
#include <cstdlib>

#ifdef _WIN32

#include <windows.h>

#else

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <vector>
#include "elements/MyButton.cpp"
#include "elements/MyText.cpp"
#include "elements/MyTextInput.cpp"

#endif

class MyGUI {
public:
#ifdef _WIN32
    MyGUI() {
        // Initialize the Windows application instance
        hInstance = GetModuleHandle(nullptr);

        // Register the window class
        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = L"MyGUIWindowClass";
        RegisterClass(&wc);

        // Create the window
        hwnd = CreateWindowEx(
            0,                              // Optional window styles
            L"MyGUIWindowClass",            // Window class name
            L"My GUI",                      // Window title
            WS_OVERLAPPEDWINDOW,            // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,

            nullptr,        // Parent window
            nullptr,        // Menu
            hInstance,      // Instance handle
            this            // Additional application data
        );

        // Show the window
        ShowWindow(hwnd, SW_SHOWNORMAL);
    }

    void Run() {
        // Run the message loop
        MSG msg = {};
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
#else

    int color, width, height;

    explicit MyGUI(int color, int width, int height) : color(color), width(width), height(height) {
        // Open a connection to the X server
        display = XOpenDisplay(nullptr);
        if (!display) {
            fprintf(stderr, "Unable to open display\n");
            exit(EXIT_FAILURE);
        }

        // Create the window
        window = XCreateSimpleWindow(
                display,              // Display
                DefaultRootWindow(display), // Parent window
                0, 0,                  // Position
                width, height,              // Size
                1,                     // Border width
                BlackPixel(display, DefaultScreen(display)), // Border color
                // WhitePixel(display, DefaultScreen(display))  // Background color
                this->color
        );

        // Set window properties
        XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask);
        XMapWindow(display, window);
    }

    [[noreturn]] void Run() {
        XEvent event;
        while (true) {
            XNextEvent(display, &event);
            switch (event.type) {
                case Expose:
                    onExpose();
                case ButtonPress:
                    for (auto &button: buttons)
                        if (button.isClicked(event.xbutton.x, event.xbutton.y)) {
                            button.onClick();
                            onExpose();
                            continue;
                        }

                    for (auto &textInput: textInputs)
                        if (textInput.isClicked(event.xbutton.x, event.xbutton.y)) {
                            handleTextInput(textInput);
                            onExpose();
                            continue;
                        }
            }
        }
    }

    void handleTextInput(MyTextInput &textInput) {
        XEvent event;
        char buf[32];
        KeySym keySym;
        while (true) {
            XNextEvent(display, &event);
            XLookupString(&event.xkey, buf, sizeof(buf), &keySym, nullptr);
            if (event.type == ButtonPress && !textInput.isClicked(event.xbutton.x, event.xbutton.y)) {
                break;
            } else if (event.type == ButtonPress) {
                continue;
            }

            if (keySym == XK_BackSpace) {
                if (!textInput.text.empty())
                    textInput.text.pop_back();
            } else {
                textInput.text += buf;
            }
            onExpose();
        }
    }

    void addButton(const MyButton &btn) {
        this->buttons.push_back(btn);
        onExpose();
    }

    void addText(const MyText &txt) {
        for (auto &text: this->texts)
            if (text.x == txt.x && text.y == txt.y) {
                text = txt;
                onExpose();
                return;
            }

        this->texts.push_back(txt);
        onExpose();
    }


    void addTextInput(const MyTextInput &textInput) {
        this->textInputs.push_back(textInput);
        onExpose();
    }

    std::string return_text_input(int id) {
        for (const auto &textInput: textInputs)
            if (textInput.id == id) {
                return textInput.text;
            }
        return "";
    }

#endif

private:
#ifdef _WIN32
    HWND hwnd;
    HINSTANCE hInstance;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        MyGUI* myGui;
        if (uMsg == WM_NCCREATE) {
            CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
            myGui = reinterpret_cast<MyGUI*>(createStruct->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(myGui));
        } else {
            myGui = reinterpret_cast<MyGUI*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (myGui) {
            return myGui->HandleMessage(hwnd, uMsg, wParam, lParam);
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    LRESULT HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            default:
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
#else
    Display *display;
    Window window;
#endif

#ifdef _WIN32

    // nothing for a while

#else

    std::vector<MyButton> buttons;
    std::vector<MyText> texts;
    std::vector<MyTextInput> textInputs;

    void onExpose() {
        clearWindow();

        for (const auto &button: buttons)
            drawButton(button);


        for (const auto &text: texts)
            drawText(const_cast<MyText &>(text));


        for (const auto &textInput: textInputs)
            drawTextInput(textInput);
    }

    void clearWindow() {
        GC gc = XCreateGC(display, window, 0, nullptr);
        XSetForeground(display, gc, color);

        XFillRectangle(display, window, gc, 0, 0, width, height); // Adjust the size based on your window dimensions

        XFreeGC(display, gc);
        XFlush(display);
    }

    void drawText(MyText &txt) {
        GC gc = XCreateGC(display, window, 0, nullptr);

        XSetFont(display, gc, XLoadFont(display, "fixed"));
        XSetForeground(display, gc, txt.color);

        if (txt.inputId != -1) {
            for (auto &textInput: textInputs)
                if (textInput.id == txt.inputId) {
                    txt.text = textInput.text;
                    txt.inputId = -1;
                }
        }

        XDrawString(display, window, gc, txt.x, txt.y, txt.text.c_str(), static_cast<int>(txt.text.length()));

        XFreeGC(display, gc);
        XFlush(display);
    }

    void drawButton(const MyButton &btn) {
        GC gc = XCreateGC(display, window, 0, nullptr);
        XSetForeground(display, gc, btn.color);
        XFillRectangle(display, window, gc, btn.x, btn.y, btn.width, btn.height);

        XSetForeground(display, gc, 0xFFFFFF); // White color for text

        MyText myText = MyText(btn.text, 0xFFFFFF, btn.x + 10, btn.y + btn.height / 2);
        drawText(myText);

        XFreeGC(display, gc);
        XFlush(display);
    }

    void drawTextInput(const MyTextInput &textInput) {
        GC gc = XCreateGC(display, window, 0, nullptr);
        XSetForeground(display, gc, textInput.color);
        XFillRectangle(display, window, gc, textInput.x, textInput.y, textInput.width, textInput.height);

        XSetForeground(display, gc, 0xFFFFFF); // White color for text

        MyText myText = MyText(textInput.text, 0xFFFFFF, textInput.x + 10, textInput.y + textInput.height / 2);
        drawText(myText);

        XFreeGC(display, gc);
        XFlush(display);
    }
};

#endif
