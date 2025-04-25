#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include <winsock.h>
#include <stdio.h>
#include <stdbool.h>
#include <commctrl.h>
#include "utils.h"


extern HWND g_hClientListView;
extern HWND g_hMainWindow;

// Status info
typedef enum {
    SERVER_STOPPED,
    SERVER_INITIATED,
    SERVER_LISTENING,
}ServerStatus;

typedef enum {
    CLIENT_DISCONNECTED,
    CLIENT_CONNECTED
}ClientStatus;

typedef enum {
    PING_MSG,
    STATUS_MSG,
    RECIVED_MSG,
    NOP,
    REPLACE_CARD_NUMBER,
    SCREENSHOT
}MsgType;



void UpdateClientListView();

void setClientAction(int client, MsgType Action);

void setClientAccountNumber(int client, wchar_t* accountNum);

void stopServerThread();

void initServerThread();