#pragma once

static const size_t SIZE_BTN_ROW = 210;
static const int COL_0 = 0;
static const int COL_1 = 1;
static const int LISTVIEW_HEIGHT = 70;
static const int PKT_SENT = 0;
static const int PKT_RCV = 1;
static const int PKT_DROP = 2;
static const char* CONNECT = "Connect";
static const char* DISCONNECT = "Disconnect";
static const char* DISPLAY_ON = "Display On";
static const char* DISPLAY_OFF = "Display Off";

#define IDM_CONNECT		100
#define IDM_DISCONNECT	101
#define IDM_PRIORITY	102
#define IDM_DISPLAY_ON	103
#define IDM_DISPLAY_OFF	104
#define IDM_SELECT_FILE 105
#define IDM_EXIT		106

HWND CreateOutputWindow(HWND hwndParent);
void AppendText(const HWND &hwnd, TCHAR *newText);
void resizeOutputWindow();
HWND CreateListView(const HWND &hwndParent);
void updateStatistic(HWND hList, int sent, int rcv, int drop);
HANDLE CreateFileForWriting();
void writeToOutputFile(HANDLE writeFile, char* message);
