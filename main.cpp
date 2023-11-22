//
// Created by yuriizin on 22.11.23.
//

// Cross-platform GUI Toolkit Example (Windows and X Window System)
// Compile with: g++ -o mygui mygui.cpp -lX11 (for X Window System)

#include <cstdio>
#include <cstdlib>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
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
                display,              // Display
                DefaultRootWindow(display), // Parent window
                0, 0,                  // Position
                800, 600,              // Size
                1,                     // Border width
                BlackPixel(display, DefaultScreen(display)), // Border color
                WhitePixel(display, DefaultScreen(display))  // Background color
        );

        // Set window properties
        XSelectInput(display, window, ExposureMask | KeyPressMask);
        XMapWindow(display, window);
    }

    void Run() {
        // Run the event loop
        XEvent event;
        while (true) {
            XNextEvent(display, &event);
            if (event.type == Expose) {
                onExpose();
            } else if (event.type == KeyPress) {
                drawString(50, 100, "lskdjflskjdfflk!", 0x000000);
            } else if (event.type == ButtonPress) {
//                if(isButtonClicked(event.xbutton.x, event.xbutton.y)){
                    drawString(50, 200, "aaaaaaaaaaaaaaaa!", 0x000000);
//                }
            }
        }
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
    Display* display;
    Window window;
#endif

#ifdef _WIN32

#else
    void onExpose() {
        drawString(10, 20, "Hello, World!", 0x000000);
        drawString(20, 30, " Bye!", 0x000000);
        drawButton(10, 40, 100, 50, "Click me!", 0x000000);
    }

    void drawString(int x, int y, const char* string, int color) {
        GC gc = XCreateGC(display, window, 0, nullptr);

        size_t strlen = std::string(string).length();

        XSetFont(display, gc, XLoadFont(display, "fixed")); // font
        XSetForeground(display, gc, color); // color

        XDrawString(display, window, gc, x, y, string, static_cast<int>(strlen));

        XFreeGC(display, gc);
        XFlush(display);
    }

    void drawButton(int x, int y, int width, int height, const char* string, int color) {
        GC gc = XCreateGC(display, window, 0, nullptr);
        // Set the color
        XSetForeground(display, gc, color);

        // Draw the button as a rectangle
        XFillRectangle(display, window, gc, x, y, width, height);

        drawString(x + 20 , y + 20, string, 0xFF0000);

        XFreeGC(display, gc);
        XFlush(display);
    }
#endif
};

int main() {
    MyGUI myGui;
    myGui.Run();
    return 0;
}

