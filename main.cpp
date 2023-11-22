//
// Created by yuriizin on 22.11.23.
//

#include <cstdio>
#include <cstdlib>

#ifdef _WIN32

#include <windows.h>

#else

#include <X11/Xlib.h>
#include "elements/MyButton.cpp"
#include "elements/MyText.cpp"

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

    MyGUI() {
        // Open a connection to the X server
        display = XOpenDisplay(nullptr);
        if (!display) {
            fprintf(stderr, "Unable to open display\n");
            exit(EXIT_FAILURE);
        }

        // Create the window
        window = XCreateSimpleWindow(
                display,              // Dis    play
                DefaultRootWindow(display), // Parent window
                0, 0,                  // Position
                800, 600,              // Size
                1,                     // Border width
                BlackPixel(display, DefaultScreen(display)), // Border color
                WhitePixel(display, DefaultScreen(display))  // Background color
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
                    if (button.isClicked(event.xbutton.x, event.xbutton.y)) {
                        button.onClick();
                        onExpose();
                    }
            }
        }
    }

    void addButton(const MyButton &btn) {
        this->button = btn;
        drawButton(btn);
    }

    void addText(const MyText &txt) {
        this->text = txt;
        drawText(txt);
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

    MyButton button;
    MyText text;

    void onExpose() {
        drawText(text);
        drawButton(button);
    }

    void drawText(const MyText &txt) {
        GC gc = XCreateGC(display, window, 0, nullptr);

        XSetFont(display, gc, XLoadFont(display, "fixed"));
        XSetForeground(display, gc, txt.color);

        XDrawString(display, window, gc, txt.x, txt.y, txt.text.c_str(), static_cast<int>(txt.text.length()));

        XFreeGC(display, gc);
        XFlush(display);
    }

    void drawButton(const MyButton &btn) {
        GC gc = XCreateGC(display, window, 0, nullptr);
        XSetForeground(display, gc, btn.color);
        XFillRectangle(display, window, gc, btn.x, btn.y, btn.width, btn.height);

        XSetForeground(display, gc, 0xFFFFFF); // White color for text
        drawText(
                MyText(btn.text, 0xFFFFFF, btn.x + 10, btn.y + btn.height / 2)
        );

        XFreeGC(display, gc);
        XFlush(display);
    }
};

#endif
