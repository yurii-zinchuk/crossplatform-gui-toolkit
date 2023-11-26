//
// Created by yuriizin on 22.11.23.
//

#include <cstdio>
#include <cstdlib>
#include "elements/MyButton.cpp"
#include "elements/MyText.cpp"
#include "elements/MyTextInput.cpp"
#include <vector>

#ifdef _WIN32

#include <windows.h>
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

class MyGUI {
public:
#ifdef _WIN32

    MyGUI(int color, int width, int height) {
        // Initialize the Windows application instance
        hInstance = GetModuleHandle(nullptr);
        activeTextInput = nullptr; // Initialize to nullptr

        // Register the window class
        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = "MyGUIWindowClass";
        RegisterClass(&wc);

        // Create the window
        hwnd = CreateWindowEx(
            0,                              // Optional window styles
            "MyGUIWindowClass",            // Window class name
            "My GUI",                      // Window title
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
    void addButton(const MyButton& button) {
        buttons.push_back(button);
        InvalidateRect(hwnd, nullptr, TRUE);
    }

    void addTextInput(const MyTextInput& textInput) {
        textInputs.push_back(textInput);
        InvalidateRect(hwnd, nullptr, TRUE);
    }

    void addText(const MyText& text) {
        for (auto& txt : texts) {
            if (txt.x == text.x && txt.y == text.y) {
                txt = text;
                InvalidateRect(hwnd, nullptr, TRUE);
                return;
            }
        }
        texts.push_back(text);
        InvalidateRect(hwnd, nullptr, TRUE);
    }
    std::string return_text_input(int id) {
        for (const auto& textInput : textInputs)
            if (textInput.id == id) {
                return textInput.text;
            }
        return "";
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
                for (auto& button : buttons)
                    if (button.isClicked(event.xbutton.x, event.xbutton.y)) {
                        button.onClick();
                        onExpose();
                        continue;
                    }

                for (auto& textInput : textInputs)
                    if (textInput.isClicked(event.xbutton.x, event.xbutton.y)) {
                        handleTextInput(textInput);
                        onExpose();
                        continue;
                    }
            }
        }
    }

    void handleTextInput(MyTextInput& textInput) {
        XEvent event;
        char buf[32];
        KeySym keySym;
        while (true) {
            XNextEvent(display, &event);
            XLookupString(&event.xkey, buf, sizeof(buf), &keySym, nullptr);
            if (event.type == ButtonPress && !textInput.isClicked(event.xbutton.x, event.xbutton.y)) {
                break;
            }
            else if (event.type == ButtonPress) {
                continue;
            }

            if (keySym == XK_BackSpace) {
                if (!textInput.text.empty())
                    textInput.text.pop_back();
            }
            else {
                textInput.text += buf;
            }
            onExpose();
        }
    }

    void addButton(const MyButton& btn) {
        this->buttons.push_back(btn);
        onExpose();
    }

    void addText(const MyText& txt) {
        for (auto& text : this->texts)
            if (text.x == txt.x && text.y == txt.y) {
                text = txt;
                onExpose();
                return;
            }

        this->texts.push_back(txt);
        onExpose();
    }


    void addTextInput(const MyTextInput& textInput) {
        this->textInputs.push_back(textInput);
        onExpose();
    }

    std::string return_text_input(int id) {
        for (const auto& textInput : textInputs)
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
    std::vector<MyButton> buttons;
    std::vector<MyTextInput> textInputs;
    std::vector<MyText> texts;
    MyTextInput* activeTextInput; // Pointer to the active text input field
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        MyGUI* myGui;
        if (uMsg == WM_NCCREATE) {
            CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
            myGui = reinterpret_cast<MyGUI*>(createStruct->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(myGui));
        }
        else {
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

        case WM_PAINT:
            onPaint();
            break;

        case WM_LBUTTONDOWN: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            for (auto& button : buttons) {
                if (button.isClicked(x, y)) {
                    button.onClick();
                    InvalidateRect(hwnd, nullptr, TRUE);
                }
            }
            for (auto& textInput : textInputs) {
                if (textInput.isClicked(x, y)) {
                    // Set this textInput as active
                    activeTextInput = &textInput;
                    InvalidateRect(hwnd, nullptr, TRUE);
                    break;  // Break after finding the clicked text input
                }
            }
            break;
        }

        case WM_CHAR: {
            if (activeTextInput) {
                if (activeTextInput && wParam == VK_BACK) {
                    if (!activeTextInput->text.empty()) {
                        activeTextInput->text.pop_back();
                    }
                }
                else {
                    char c = static_cast<char>(wParam);
                    activeTextInput->text += c;
                }
                InvalidateRect(hwnd, nullptr, TRUE);


            }
            break;
        }

                    //        case WM_KEYDOWN: {
                      //          if (activeTextInput && wParam == VK_BACK) {
                        //            if (!activeTextInput->text.empty()) {
                          //              activeTextInput->text.pop_back();
                            //            InvalidateRect(hwnd, nullptr, TRUE);
                              //      }
                           //     }
                             //   break;
                            //}

                                        // Other cases for additional message handling
                                        // ...

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        return 0;
    }

    void onPaint() {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Create an off-screen buffer for double buffering
        HDC hdcBuffer = CreateCompatibleDC(hdc);
        HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, 800, 600);
        HGDIOBJ oldBuffer = SelectObject(hdcBuffer, hbmBuffer);

        // Create a solid brush for the background color of buttons and text inputs
        HBRUSH backgroundBrush = CreateSolidBrush(RGB(240, 240, 240));

        // Clear the entire buffer with the window's background color (white in this case)
        RECT entireRect;
        GetClientRect(hwnd, &entireRect);
        HBRUSH windowBackgroundBrush = CreateSolidBrush(RGB(255, 255, 255)); // White background
        FillRect(hdcBuffer, &entireRect, windowBackgroundBrush);

        // Draw buttons with background color
        for (const auto& button : buttons) {
            RECT rect = { button.x, button.y, button.x + button.width, button.y + button.height };
            FillRect(hdcBuffer, &rect, backgroundBrush);
            DrawText(hdcBuffer, button.text.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }

        // Draw text inputs with background color
        for (const auto& textInput : textInputs) {
            RECT rect = { textInput.x, textInput.y, textInput.x + textInput.width, textInput.y + textInput.height };
            FillRect(hdcBuffer, &rect, backgroundBrush);
            DrawText(hdcBuffer, textInput.text.c_str(), -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }

        // Draw texts
        for (const auto& text : texts) {
            SetTextColor(hdcBuffer, text.color);
            TextOut(hdcBuffer, text.x, text.y, text.text.c_str(), text.text.length());
        }

        // Copy the buffer to the screen
        BitBlt(hdc, 0, 0, 800, 600, hdcBuffer, 0, 0, SRCCOPY);

        // Clean up
        SelectObject(hdcBuffer, oldBuffer); // Restore old buffer
        DeleteObject(hbmBuffer);
        DeleteObject(backgroundBrush);
        DeleteObject(windowBackgroundBrush);
        DeleteDC(hdcBuffer);

        EndPaint(hwnd, &ps);
    }
};

// Application entry point

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
