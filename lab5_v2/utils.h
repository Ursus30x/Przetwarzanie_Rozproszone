// utils.h

#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>

// Deklaracja zewnêtrzna globalnej zmiennej g_hMainWindow
extern HWND g_hMainWindow;

#ifdef _DEBUG

#define LOG(msg, ...)    log_message(stdout, "LOG", msg, __VA_ARGS__)
#define WARN(msg, ...)   log_message(stdout, "WARN", msg, __VA_ARGS__)
#define ERROR(msg, ...)  log_message(stderr, "ERROR", msg, __VA_ARGS__); fprintf(stderr, "Press enter to exit...\n"); getchar(); exit(1);


// Forward declaration of WriteToTerminal
void WriteToTerminal(const wchar_t* message);

static inline void log_message(FILE* stream, const char* level, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    // Drukuj do konsoli (stdout/stderr)
    fprintf(stream, "[%s] ", level);
    vfprintf(stream, fmt, args);
    fprintf(stream, "\n");

    // Przekazuj do terminala tylko jeœli g³ówne okno istnieje
    if (g_hMainWindow) {
        // Sk³adamy wiadomoœæ do tymczasowego bufora w ANSI
        char ansiBuffer[1024];
        vsnprintf(ansiBuffer, sizeof(ansiBuffer), fmt, args);

        // Konwertujemy poziom logowania
        wchar_t levelPrefix[16];
        mbstowcs(levelPrefix, level, sizeof(levelPrefix) / sizeof(wchar_t));

        // Konwertujemy ANSI bufor do wide char
        wchar_t wideBuffer[1024];
        mbstowcs(wideBuffer, ansiBuffer, sizeof(wideBuffer) / sizeof(wchar_t));

        // Finalna wiadomoœæ
        wchar_t finalMessage[1104];
        swprintf(finalMessage, sizeof(finalMessage) / sizeof(wchar_t), L"[%s] %s\n", levelPrefix, wideBuffer);

        WriteToTerminal(finalMessage);
    }

    va_end(args);  // Po wszystkim
}


static inline void EnableConsole()
{
    AllocConsole();

    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    LOG("Console attached!\n");
}

#else

static inline void log_message(FILE* stream, const char* level, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    // Przekazuj do terminala tylko jeœli g³ówne okno istnieje
    if (g_hMainWindow) {
        // Sk³adamy wiadomoœæ do tymczasowego bufora w ANSI
        char ansiBuffer[1024];
        vsnprintf(ansiBuffer, sizeof(ansiBuffer), fmt, args);

        // Konwertujemy poziom logowania
        wchar_t levelPrefix[16];
        mbstowcs(levelPrefix, level, sizeof(levelPrefix) / sizeof(wchar_t));

        // Konwertujemy ANSI bufor do wide char
        wchar_t wideBuffer[1024];
        mbstowcs(wideBuffer, ansiBuffer, sizeof(wideBuffer) / sizeof(wchar_t));

        // Finalna wiadomoœæ
        wchar_t finalMessage[1104];
        swprintf(finalMessage, sizeof(finalMessage) / sizeof(wchar_t), L"[%s] %s\n", levelPrefix, wideBuffer);

        WriteToTerminal(finalMessage);
    }

    va_end(args);  // Po wszystkim
}


void EnableConsole()
{
    return;
}

#define LOG(msg, ...)    log_message(stdout, "LOG", msg, __VA_ARGS__)
#define WARN(msg, ...)   log_message(stdout, "WARN", msg, __VA_ARGS__) 
#define ERROR(msg) 

#endif




static inline void ConvertWcharToChar(const wchar_t* wstr, char* str, int strLen) {
    // Funkcja WideCharToMultiByte konwertuje szerokoznakowy ci¹g (wchar_t) na ci¹g w char
   WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, strLen, NULL, NULL);
}
