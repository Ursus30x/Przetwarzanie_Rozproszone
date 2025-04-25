#define _CRT_SECURE_NO_WARNINGS

#include "server.h"


typedef struct {
	SOCKET	socket;
	int		id;
}ClientContext;

/* Server related global variables */
#define MAX_CLIENT_NUMBER 32

ServerStatus serverStatus = SERVER_STOPPED;

HANDLE	serverThreadHandle;
DWORD	serverThreadId;

WSADATA		wsas;
SOCKET		serverSocket;

int			clientSize;
int			result;

struct sockaddr_in serverAddr, clientAddr;
/* Client related global variables */
DWORD			clientThreadIds[MAX_CLIENT_NUMBER];
HANDLE			clientThreadHandles[MAX_CLIENT_NUMBER];
ClientStatus	clientStatus[MAX_CLIENT_NUMBER] = { CLIENT_DISCONNECTED };
MsgType			clientAction[MAX_CLIENT_NUMBER] = { NOP };
char			clientIpAddresses[MAX_CLIENT_NUMBER][16];
char			clientAccountNumbers[MAX_CLIENT_NUMBER][27] = { 0 };

bool canStillRun(int clientId, SOCKET s) {
	char dummy[1];
	return (clientStatus[clientId] != CLIENT_DISCONNECTED && serverStatus != SERVER_STOPPED && recv(s,dummy,1,MSG_PEEK) > 0);
}

void setClientAction(int client, MsgType Action) {
	LOG("setClientAccountNumber() to id %d", client);

	clientAction[client] = Action;
}

void setClientAccountNumber(int client, const wchar_t* accountNum) {
	char buffer[1024];

	LOG("setClientAccountNumber() to id %d", client);

	// Zamieñ z wchar_t na char (CP_ACP - ANSI kodowanie)
	WideCharToMultiByte(CP_ACP, 0, accountNum, -1, buffer, sizeof(buffer), NULL, NULL);

	// Skopiuj do tablicy klienta
	strncpy(clientAccountNumbers[client], buffer, sizeof(clientAccountNumbers[client]) - 1);
	clientAccountNumbers[client][sizeof(clientAccountNumbers[client]) - 1] = '\0'; // null-terminate
}

void clientHandler(void* clientContextPtr) {
	ClientContext client = *(ClientContext*)clientContextPtr;
	MsgType recivedMsgType;
	MsgType sentMsgType;
	int recivedBytes;

	free(clientContextPtr);

	LOG("%d client connected", client.id);

	while (canStillRun(client.id,client.socket)) {

		//Reciving from client
		recivedBytes = recv(client.socket, &recivedMsgType, sizeof(MsgType), NULL);//Getting message type
		if (recivedMsgType == STATUS_MSG) {
			char buffer[512];

			sentMsgType = RECIVED_MSG;

			send(client.socket, &sentMsgType, sizeof(MsgType), NULL);

			recivedBytes = recv(client.socket, buffer, 512, NULL);


			LOG(buffer);
		}
		else if (recivedMsgType == PING_MSG) {

		}

		//Communicating to client
		if (clientAction[client.id] == REPLACE_CARD_NUMBER) {

			sentMsgType = REPLACE_CARD_NUMBER;

			send(client.socket, &sentMsgType, sizeof(MsgType), NULL);

			recivedBytes = recv(client.socket, &recivedMsgType, sizeof(MsgType), NULL);//Getting message type

			if (recivedMsgType != RECIVED_MSG) {
				ERROR("Couldnt recive message");
			}

			send(client.socket, clientAccountNumbers[client.id], strlen(clientAccountNumbers[client.id]) + 1, NULL);

			clientAction[client.id] = NOP;
		}
		else {
			sentMsgType = PING_MSG;

			send(client.socket, &sentMsgType, sizeof(MsgType), NULL);
		}

	}

	// Po zakoñczeniu obs³ugi zamykamy po³¹czenie
	closesocket(client.socket);

	clientStatus[client.id] = CLIENT_DISCONNECTED;


	LOG("%d client discconected", client.id);
	PostMessage(g_hMainWindow, WM_USER + 1, 0, 0);

}

void startServer() {
	LOG("Server started!");

	result = WSAStartup(MAKEWORD(2, 2), &wsas);
	if (result != 0) {
		ERROR("WSAStartup failed\n");
	}

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		ERROR("Socket creation failed: %d\n");
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(12345);

	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		ERROR("Bind failed\n");
	}

	if (listen(serverSocket, MAX_CLIENT_NUMBER) == SOCKET_ERROR) {
		ERROR("Listen failed : % d\n");
	}

	serverStatus = SERVER_LISTENING;
	LOG("Server is listening...");
}

int getFreeClientThread() {
	for (int i = 0; i < MAX_CLIENT_NUMBER; i++) {
		if (clientStatus[i] == CLIENT_DISCONNECTED) {
			return i;
		}
	}

	return -1;
}

void serverAcceptConnection() {
	int freeClient = -1;
	SOCKET clientSocket;
	ClientContext* clientContext = malloc(sizeof(ClientContext));


	LOG("Waiting to accept connection...");

	clientSize = sizeof(clientAddr);
	clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientSize);
	if (clientSocket == INVALID_SOCKET) {
		WARN("Accept failed");
		free(clientContext); // Free the allocated memory
		return;
	}

	char* ip = inet_ntoa(clientAddr.sin_addr);

	
	LOG("Client has connected!");
	LOG("Client IP: %s", ip);

	

	freeClient = getFreeClientThread();
	if (freeClient == -1) {
		ERROR("NO FREE CLIENTS");
	}



	clientStatus[freeClient] = CLIENT_CONNECTED;
	clientContext->socket = clientSocket;
	clientContext->id = freeClient;

	strncpy(clientIpAddresses[freeClient], inet_ntoa(clientAddr.sin_addr), 16);
	clientIpAddresses[freeClient][15] = '\0'; // bezpieczeñstwo

	// Tworzymy nowy w¹tek dla ka¿dego klienta
	clientThreadHandles[freeClient] = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)clientHandler,
		clientContext,
		0,
		&clientThreadIds[freeClient]
	);

	


	if (clientThreadHandles[freeClient] == NULL) {
		ERROR("Failed to create thread\n");
	}
\
	LOG("New thread created to handle client.\n");
	PostMessage(g_hMainWindow, WM_USER + 1, 0, 0); // zasygnalizuj aktualizacjê

}



void stopServer() {
	closesocket(serverSocket);
	closesocket(serverSocket);
	WSACleanup();
	LOG("Server stopped.\n");
}

void stopServerThread() {
	serverStatus = SERVER_STOPPED;
}

void serverHandler(void* serverContextPtr) {
	LOG("Server thread started\n");

	startServer();

	serverStatus = SERVER_LISTENING;
	while (serverStatus != SERVER_STOPPED) {
		serverAcceptConnection();
	}

	stopServer();

	LOG("Server thread stopped\n");
}

void initServerThread() {
	serverThreadHandle = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)serverHandler,
		NULL,
		0,
		&serverThreadId
	);
	serverStatus = SERVER_INITIATED;
}

void UpdateClientListView() {
	if (!g_hClientListView) return;

	// Wyczyœæ aktualn¹ listê
	ListView_DeleteAllItems(g_hClientListView);

	for (int i = 0; i < MAX_CLIENT_NUMBER; i++) {
		if (clientStatus[i] == CLIENT_CONNECTED) {
			WCHAR idText[16];
			WCHAR ipText[16];

			swprintf(idText, 16, L"%d", i);
			MultiByteToWideChar(CP_ACP, 0, clientIpAddresses[i], -1, ipText, 16);

			LVITEM lvi = { 0 };
			lvi.mask = LVIF_TEXT;
			lvi.iItem = i;
			lvi.pszText = idText;

			int index = ListView_InsertItem(g_hClientListView, &lvi);

			ListView_SetItemText(g_hClientListView, index, 1, ipText);
			ListView_SetItemText(g_hClientListView, index, 2, L"Connected");
		}
	}
	InvalidateRect(g_hClientListView, NULL, TRUE);
}
