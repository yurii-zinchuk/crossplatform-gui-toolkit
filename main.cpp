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
#include <stdexcept>
#include <iostream>

#endif

class MyGUI {
public:
#ifdef _WIN32
    int width;
    int height;
    int color;
    int n_color;
    MyGUI(int color, int n_color, int width, int height) {
        this->width = width;
        this->height = height;
        this->color = color;
        this->n_color = n_color;

        hInstance = GetModuleHandle(nullptr);
        activeTextInput = nullptr;

        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = "MyGUIWindowClass";
        RegisterClass(&wc);

        hwnd = CreateWindowEx(
            0,
            "MyGUIWindowClass",
            "My GUI",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, width, height,
            nullptr,
            nullptr,
            hInstance,
            this
        );

        ShowWindow(hwnd, SW_SHOWNORMAL);
    }

    void Run() {
        MSG msg = {};
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void addButton(MyButton& button) {
        bool rel_width = button.relative_size == 1 || button.relative_size == 3;
        bool rel_height = button.relative_size == 2 || button.relative_size == 3;

        button.width = rel_width ? width * button.initial_width / 100 : button.width;
        button.height = rel_height ? button.initial_height * height / 100 : button.height;
        buttons.push_back(button);
        InvalidateRect(hwnd, nullptr, TRUE);
    }

    void addTextInput(MyTextInput& textInput) {
        bool rel_width = textInput.relative_size == 1 || textInput.relative_size == 3;
        bool rel_height = textInput.relative_size == 2 || textInput.relative_size == 3;

        textInput.width = rel_width ? textInput.initial_width * width / 100 : textInput.width;
        textInput.height = rel_height ? textInput.initial_height * height / 100 : textInput.height;
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
    int color, n_color, width, height;

    explicit MyGUI(int color, int n_color, int width, int height) : color(color), n_color(n_color), width(width),
                                                                    height(height) {
        display = XOpenDisplay(nullptr);
        if (!display) {
            fprintf(stderr, "Unable to open display\n");
            exit(EXIT_FAILURE);
        }

        window = XCreateSimpleWindow(
                display,
                DefaultRootWindow(display),
                0, 0,
                width, height,
                1,
                BlackPixel(display, DefaultScreen(display)),
                this->color
        );

        XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);
        XMapWindow(display, window);
    }

    [[noreturn]] void Run() {
        XEvent event;
        while (true) {
            XNextEvent(display, &event);
            switch (event.type) {
                case ConfigureNotify:
                    width = event.xconfigure.width;
                    height = event.xconfigure.height;
                    onExpose();
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
    std::vector<MyButton> buttons;
    std::vector<MyTextInput> textInputs;
    std::vector<MyText> texts;
    MyTextInput* activeTextInput;
    bool IsDark = false;

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

        case WM_SIZE:
            width = LOWORD(lParam);
            height = HIWORD(lParam);

            InvalidateRect(hwnd, NULL, TRUE);
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
                    activeTextInput = &textInput;
                    InvalidateRect(hwnd, nullptr, TRUE);
                    break;
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

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }

        return 0;
    }


    bool IsWindowsDarkThemeActive() {
        DWORD themeData;
        DWORD dataSize = sizeof(themeData);
        LONG result;
        HKEY hKey;

        // Open the registry key where the theme data is stored
        result = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey);
        if (result != ERROR_SUCCESS) {
            return false; // Assuming light theme if we fail to read the registry
        }

        // Query the value for the AppsUseLightTheme setting
        result = RegQueryValueExW(hKey, L"AppsUseLightTheme", 0, NULL, reinterpret_cast<LPBYTE>(&themeData), &dataSize);
        if (result != ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return false; // Assuming light theme if we fail to read the registry
        }

        // Close the registry key
        RegCloseKey(hKey);

        // If the value is 0, dark theme is active, otherwise it's light
        return themeData == 0;
    }

    void onPaint() {
        IsDark = IsWindowsDarkThemeActive();
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        HDC hdcBuffer = CreateCompatibleDC(hdc);
        HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, width, height);
        SelectObject(hdcBuffer, hbmBuffer);
        RECT entireRect;
        GetClientRect(hwnd, &entireRect);
        HBRUSH backgroundBrush = CreateSolidBrush(IsDark ? n_color : color);
        FillRect(hdcBuffer, &entireRect, backgroundBrush);

        for (const auto& textInput : textInputs) {
            RECT rect = { textInput.x, textInput.y, textInput.x + textInput.width, textInput.y + textInput.height };
            backgroundBrush = CreateSolidBrush(IsDark ? textInput.n_color : textInput.color);
            FillRect(hdcBuffer, &rect, backgroundBrush);
            SetTextColor(hdcBuffer, IsDark ? textInput.text_n_color : textInput.text_color);
            SetBkColor(hdcBuffer, 0xFF72F7);
            SetBkMode(hdcBuffer, TRANSPARENT);
            DrawText(hdcBuffer, textInput.text.c_str(), -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }

        for (const auto& button : buttons) {
            RECT rect = { button.x, button.y, button.x + button.width, button.y + button.height };
            backgroundBrush = CreateSolidBrush(IsDark ? button.n_color : button.color);
            FillRect(hdcBuffer, &rect, backgroundBrush);
            SetTextColor(hdcBuffer, IsDark ? button.text_n_color : button.text_color);
            SetBkMode(hdcBuffer, TRANSPARENT);
            DrawText(hdcBuffer, button.text.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        }
        BitBlt(hdc, 0, 0, width, height, hdcBuffer, 0, 0, SRCCOPY);


        // Draw buttons
        for (auto& button : buttons) {
            bool rel_width = button.relative_size == 1 || button.relative_size == 3;
            bool rel_height = button.relative_size == 2 || button.relative_size == 3;

            button.width = rel_width ? width * button.initial_width / 100 : button.width;
            button.height = rel_height ? button.initial_height * height / 100 : button.height;

            RECT rect = { button.x, button.y, button.x + button.width, button.y + button.height };
            backgroundBrush = CreateSolidBrush(IsDark ? button.n_color : button.color);
            FillRect(hdcBuffer, &rect, backgroundBrush);
            SetTextColor(hdcBuffer, IsDark ? button.text_n_color : button.text_color);
            SetBkMode(hdcBuffer, TRANSPARENT);

            DrawText(hdcBuffer, button.text.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        }

        // Draw text inputs
        for (auto& textInput : textInputs) {
            bool rel_width = textInput.relative_size == 1 || textInput.relative_size == 3;
            bool rel_height = textInput.relative_size == 2 || textInput.relative_size == 3;

            textInput.width = rel_width ? textInput.initial_width * width / 100 : textInput.width;
            textInput.height = rel_height ? textInput.initial_height * height / 100 : textInput.height;

            RECT rect = { textInput.x, textInput.y, textInput.x + textInput.width, textInput.y + textInput.height };
            backgroundBrush = CreateSolidBrush(IsDark ? textInput.n_color : textInput.color);
            FillRect(hdcBuffer, &rect, backgroundBrush);
            SetTextColor(hdcBuffer, IsDark ? textInput.text_n_color : textInput.text_color);
            SetBkColor(hdcBuffer, IsDark ? textInput.n_color : textInput.color);
            SetBkMode(hdcBuffer, TRANSPARENT);
            DrawText(hdcBuffer, textInput.text.c_str(), -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }

        // Draw texts
        for (auto& text : texts) {
            if (text.inputId != -1) {
                for (auto& textInput : textInputs) {
                    if (textInput.id == text.inputId) {
                        text.text = textInput.text;
                        text.inputId = -1;
                    }
                }
            }

            SetTextColor(hdc, IsDark ? text.n_color : text.color);
            SetBkMode(hdc, TRANSPARENT);
            TextOut(hdc, text.x, text.y, text.text.c_str(), text.text.length());
        }

        DeleteObject(hbmBuffer);
        DeleteDC(hdcBuffer);

        EndPaint(hwnd, &ps);
    }
};
#else
    Display *display;
    Window window;
#endif

#ifdef _WIN32

   // nothing

#else

    std::vector<MyButton> buttons;
    std::vector<MyText> texts;
    std::vector<MyTextInput> textInputs;
    bool isDark = false;

    bool isDarkTheme() {
        char buffer[128];
        std::string result;
        FILE *pipe = popen("gsettings get org.gnome.desktop.interface gtk-theme", "r");
        if (!pipe) throw std::runtime_error("popen() failed!");
        try {
            while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
                result += buffer;
            }
        } catch (...) {
            pclose(pipe);
            throw;
        }
        pclose(pipe);

        return result.find("dark") != std::string::npos;
    }

    void onExpose() {
        isDark = isDarkTheme();

        clearWindow();

        for (auto &button: buttons)
            drawButton(button);

        for (const auto &text: texts)
            drawText(const_cast<MyText &>(text));

        for (auto &textInput: textInputs)
            drawTextInput(textInput);
    }

    void clearWindow() {
        GC gc = XCreateGC(display, window, 0, nullptr);
        XSetForeground(display, gc, isDark ? n_color : color);

        XFillRectangle(display, window, gc, 0, 0, width, height);

        XFreeGC(display, gc);
        XFlush(display);
    }

    void drawText(MyText &txt) {
        GC gc = XCreateGC(display, window, 0, nullptr);

        XSetFont(display, gc, XLoadFont(display, "fixed"));
        XSetForeground(display, gc, isDark ? txt.n_color : txt.color);

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

    void drawButton(MyButton &btn) {
        bool rel_width = btn.relative_size == 1 || btn.relative_size == 3;
        bool rel_height = btn.relative_size == 2 || btn.relative_size == 3;

        btn.width = rel_width ? width * btn.initial_width / 100 : btn.width;
        btn.height = rel_height ? btn.initial_height * height / 100 : btn.height;

        GC gc = XCreateGC(display, window, 0, nullptr);
        XSetForeground(display, gc, isDark ? btn.n_color : btn.color);
        XFillRectangle(display, window, gc, btn.x, btn.y, btn.width, btn.height);

        XSetForeground(display, gc, isDark ? btn.text_n_color : btn.text_color);

        MyText myText = MyText(btn.text, btn.text_color, btn.text_n_color, btn.x + 10, btn.y + btn.height / 2);
        drawText(myText);

        XFreeGC(display, gc);
        XFlush(display);
    }

    void drawTextInput(MyTextInput &textInput) {
        bool rel_width = textInput.relative_size == 1 || textInput.relative_size == 3;
        bool rel_height = textInput.relative_size == 2 || textInput.relative_size == 3;

        textInput.width = rel_width ? textInput.initial_width * width / 100 : textInput.width;
        textInput.height = rel_height ? textInput.initial_height * height / 100 : textInput.height;

        GC gc = XCreateGC(display, window, 0, nullptr);
        XSetForeground(display, gc, isDark ? textInput.n_color : textInput.color);
        XFillRectangle(display, window, gc, textInput.x, textInput.y, textInput.width, textInput.height);

        XSetForeground(display, gc, isDark ? textInput.text_n_color : textInput.text_color);

        MyText myText = MyText(textInput.text, textInput.text_color, textInput.text_n_color, textInput.x + 10,
                               textInput.y + textInput.height / 2);
        drawText(myText);

        XFreeGC(display, gc);
        XFlush(display);
    }
};

#endif
