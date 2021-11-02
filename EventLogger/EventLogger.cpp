#include <Windows.h>
#include <iostream>

#define IDC_BUTTON   1500

// for new style
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

LPCWSTR logTxt = TEXT("Log message...");

#define ADDPOPUPMENU(hmenu, string) \
HMENU hSubMenu = CreatePopupMenu(); \
AppendMenu(hmenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, string);

#define ADDMENUITEM(hmenu, ID, string) \
AppendMenu(hSubMenu, MF_STRING, ID, string);

enum
{
    IDM_EXIT,
    IDM_ABOUT
};

void CreateMainMenu(HWND hWnd)
{
    HMENU hMenu = CreateMenu();
    ADDPOPUPMENU(hMenu, TEXT("&Menu"));
    ADDMENUITEM(hMenu, IDM_EXIT, TEXT("&Exit"));
    ADDMENUITEM(hMenu, IDM_ABOUT, TEXT("&About"));
    SetMenu(hWnd, hMenu);
}

void Log(HWND hWnd, LPCWSTR msg)
{
    logTxt = msg;
    InvalidateRect(hWnd, nullptr, true);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        RECT Rect;
        GetClientRect(hWnd, &Rect);

        HINSTANCE hIns = ((LPCREATESTRUCT)lParam)->hInstance;
        HWND hButton = CreateWindowEx(WS_EX_CLIENTEDGE, 
            TEXT("button"), 
            TEXT("Click Me!"), 
            WS_CHILD | WS_VISIBLE | WS_BORDER, 
            (Rect.right - Rect.left) / 2 - 80, (Rect.bottom - Rect.top) / 2 - 20, 160, 40,
            hWnd, 
            (HMENU)IDC_BUTTON, 
            hIns, 
            nullptr
        );

        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT Rect;
        GetClientRect(hWnd, &Rect);
        TextOut(hdc, Rect.left + 5, Rect.bottom - 20, logTxt, wcslen(logTxt));
        EndPaint(hWnd, &ps);
        
        break;
    }

    case WM_KEYDOWN:
    {
        Log(hWnd, TEXT("Keyboard btn clicked."));
        break;
    }

    case WM_MOUSEWHEEL:
    {
        Log(hWnd, TEXT("Mouse wheel clicked."));
        break;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
            case IDM_ABOUT:
            {
                Log(hWnd, TEXT("About btn clicked."));
                MessageBox(nullptr, TEXT("Artyom Lazyan\nLevelUp"), TEXT("About"), MB_OK);
                break;
            }
            case IDM_EXIT:
            {
                DestroyWindow(hWnd);
                break;
            }

            case IDC_BUTTON:
            {
                Log(hWnd, TEXT("Click me btn clicked."));
                SetFocus(hWnd);
                break;
            }
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }

        break;
    }
    
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return EXIT_SUCCESS;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    MSG msg;
    HWND hWnd;
    WNDCLASSEX wc;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon =   LoadIcon(hInstance, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(wc.hInstance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = TEXT("Event Logger");

    RegisterClassEx(&wc);

    hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, 
        wc.lpszClassName, 
        TEXT("app"), 
        WS_OVERLAPPEDWINDOW, 
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        nullptr, 
        nullptr, 
        hInstance, 
        nullptr
    );

    if (!hWnd)
    {
        MessageBox(nullptr, TEXT("Failed to create the window"), TEXT("ERROR"), MB_OK);
        return EXIT_FAILURE;
    }

    CreateMainMenu(hWnd);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}