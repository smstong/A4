#include <windows.h>

#define IDC_BUTTON1 1001

const TCHAR* g_szClassName = TEXT("myWindowClass");

HWND CreateButton(HWND parent, int controlID, TCHAR* text, int x, int y, int width, int height)
{
    HWND hwndButton = CreateWindow(
        TEXT("BUTTON"),  // Predefined class; Unicode assumed 
        text,      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        x,         // x position 
        y,         // y position 
        width,        // Button width
        height,        // Button height
        parent,     // Parent window
        (HMENU)controlID,       // Control ID
        (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.
    return hwndButton;
}

typedef struct _Button
{
    HWND hWnd;
    int controlID;
    TCHAR* text;
}Button;

Button btns[] = {
    {NULL, IDC_BUTTON1, TEXT("Red") },
    {NULL, IDC_BUTTON1 + 1, TEXT("Blue") },
    {NULL, IDC_BUTTON1 + 2, TEXT("Green") },
};

LRESULT CALLBACK OnCreate(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    for (int i = 0; i <sizeof(btns)/sizeof(btns[0]); i++) {
        btns[i].hWnd = CreateButton(hwnd, btns[i].controlID, btns[i].text, 0, 40*i, 100, 30);
    }
    return 0;
}

LRESULT CALLBACK OnCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    DWORD ctrID = LOWORD(wParam);
    DWORD notifyCode = HIWORD(wParam);

    if (notifyCode == BN_CLICKED && ctrID == IDC_BUTTON1) {
        MessageBox(NULL, TEXT("click"), TEXT("Info"), MB_OK);
    }
    return TRUE;
}

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        return OnCreate(hwnd, msg, wParam, lParam);
    case WM_COMMAND:
        return OnCommand(hwnd, msg, wParam, lParam);
    case WM_LBUTTONDOWN:
        for (int i = 0; i < sizeof(btns) / sizeof(btns[0]); i++) {
            RECT rect;
            GetWindowRect(btns[i].hWnd, &rect);
            
            MoveWindow(btns[i].hWnd, 
                rect.left, 
                rect.top, 
                rect.right - rect.left, 
                rect.bottom - rect.top, 
                TRUE);
        }
        break;
    case WM_SIZE:
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    case WM_PAINT:
    {
        static PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        HDC hdc = ps.hdc;
        RECT rect;
        GetClientRect(hwnd, &rect);
        MoveToEx(hdc, rect.right/2, 0, NULL);
        LineTo(hdc, rect.right / 2, rect.bottom);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    //Step 1: Registering the Window Class
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("Error!"),
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        TEXT("The title of my window"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("Error!"),
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}