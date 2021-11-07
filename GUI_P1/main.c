#include <windows.h>
#include <process.h>
#include <tchar.h>
#include <time.h>

#define IDC_BUTTON_RESET_GAME 2001
#define IDC_BUTTON_START_GAME_SYNC 2002
#define IDC_BUTTON_START_GAME_NONSYNC 2003
#define IDC_BUTTON1 1001

typedef struct _Car
{
    HWND hWnd;
    int controlID;
    TCHAR* text;
}Car;

const TCHAR* g_szClassName = TEXT("myWindowClass");
static CRITICAL_SECTION csCalWinner;
static BOOL g_IsSyncMode = FALSE;
static BOOL g_HasSomeoneWon = FALSE;

static Car RaceCars[] = {
    {NULL, IDC_BUTTON1, TEXT("Red") },
    {NULL, IDC_BUTTON1 + 1, TEXT("Blue") },
    {NULL, IDC_BUTTON1 + 2, TEXT("Green") },
    {NULL, IDC_BUTTON1 + 3, TEXT("Yellow") },
    {NULL, IDC_BUTTON1 + 4, TEXT("Orange") },
};

// MessageBox version of "printf"
void MessageBoxF(TCHAR* szCaption, TCHAR* format, ...)
{
    TCHAR szBuf[1024];
    memset(szBuf, 0, sizeof(szBuf));
    va_list args;

    va_start(args, format);
    _vsntprintf_s(szBuf, sizeof(szBuf) / sizeof(TCHAR), 1023, format, args);
    va_end(args);

    MessageBox(NULL, szBuf, szCaption, 0);
}

static HWND CreateButton(HWND parent, int controlID, TCHAR* text, int x, int y, int width, int height)
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

static LRESULT CALLBACK OnCreate(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Init CS
    InitializeCriticalSection(&csCalWinner);

    // Init rand
    srand(time(NULL));

    // Init UI
    CreateButton(hwnd,
        IDC_BUTTON_RESET_GAME,
        TEXT("RESET"),
        0, 20,
        150, 50);
    CreateButton(hwnd,
        IDC_BUTTON_START_GAME_SYNC,
        TEXT("Start (Sync)"),
        160*1, 20,
        150, 50);
    CreateButton(hwnd,
        IDC_BUTTON_START_GAME_NONSYNC,
        TEXT("Start (Non-Sync)"),
        160*2, 20,
        150, 50);

    for (int i = 0; i <sizeof(RaceCars)/sizeof(RaceCars[0]); i++) {
        RaceCars[i].hWnd = CreateButton(hwnd,
            RaceCars[i].controlID, 
            RaceCars[i].text, 
            20, 200+60*i, 
            200, 50);
    }
 
    return 0;
}

static LRESULT CALLBACK OnDestroy(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Del CS
    DeleteCriticalSection(&csCalWinner);
    return TRUE;
}


static int CheckWiner(Car* car, int pos, int finishLine)
{
    // Check if I won
    if (g_IsSyncMode) {
        EnterCriticalSection(&csCalWinner);
    }

    if(pos > finishLine){
        if (g_HasSomeoneWon == FALSE) {
            // simulate delay, so others may NOT notice I won
            Sleep(100);
            g_HasSomeoneWon = TRUE;
            MessageBoxF(TEXT("Winner"), TEXT("%s won"), car->text);
        }
    }

    if (g_IsSyncMode) {
        LeaveCriticalSection(&csCalWinner);
    }

    return g_HasSomeoneWon;
}
static void RacingThread(void* state)
{
    Car* car = (Car*)state;
    RECT carRect;
    RECT winRect;
    while (TRUE) {

        GetWindowRect(car->hWnd, &carRect);
        MapWindowPoints(HWND_DESKTOP, GetParent(car->hWnd), (LPPOINT)&carRect, 2);
        GetClientRect(GetParent(car->hWnd), &winRect);

        if (CheckWiner(car, carRect.left, (winRect.right - winRect.left) / 2) == TRUE) {
            break;
        }

        // Move car
        MoveWindow(car->hWnd,
            carRect.left + rand()%10,
            carRect.top,
            carRect.right - carRect.left,
            carRect.bottom - carRect.top,
            TRUE);
        // simulate time used to move
        Sleep(rand()%20);
    }
}

static void StartRace(BOOL _isSync)
{
    g_IsSyncMode = _isSync;
    for (int i = 0; i < sizeof(RaceCars) / sizeof(RaceCars[0]); i++) {
        _beginthread(RacingThread, 0, &RaceCars[i]);
    }
}
static void ResetGame()
{
    g_IsSyncMode = FALSE;
    g_HasSomeoneWon = FALSE;

    for (int i = 0; i < sizeof(RaceCars) / sizeof(RaceCars[0]); i++) {
        MoveWindow(RaceCars[i].hWnd,
            20, 200 + 60 * i,
            200, 50, TRUE);
    }
}
static LRESULT CALLBACK OnLeftButtonDown(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    StartRace(FALSE);
}
static LRESULT CALLBACK OnRightButtonDown(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    StartRace(TRUE);
}
static LRESULT CALLBACK OnCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    DWORD ctrID = LOWORD(wParam);
    DWORD notifyCode = HIWORD(wParam);

    if (notifyCode == BN_CLICKED && ctrID == IDC_BUTTON_START_GAME_NONSYNC) {
        StartRace(FALSE);
    }
    if (notifyCode == BN_CLICKED && ctrID == IDC_BUTTON_START_GAME_SYNC) {
        StartRace(TRUE);
    }
    if (notifyCode == BN_CLICKED && ctrID == IDC_BUTTON_RESET_GAME) {
        ResetGame();
    }
    return TRUE;
}

// Step 4: the Window Procedure
static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        return OnCreate(hwnd, msg, wParam, lParam);
    case WM_COMMAND:
        return OnCommand(hwnd, msg, wParam, lParam);
    case WM_LBUTTONDOWN:
        OnLeftButtonDown(hwnd, msg, wParam, lParam);
        break;
    case WM_RBUTTONDOWN:
        OnRightButtonDown(hwnd, msg, wParam, lParam);
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
        OnDestroy(hwnd, msg, wParam, lParam);
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