#define STRICT
#include <windows.h>
#include "resource.h"
#include <commctrl.h>
#include "source.h"
#include <stdio.h>

/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Session.cpp - ********************
-- PROGRAM: RFID
--
-- FUNCTIONS:
--     void AppendText(const HWND &hwnd, TCHAR *message)
--
-- DATE:	   23/11/2015
-- REVISIONS:  (Date and Description)
-- DESIGNER:   **************
-- PROGRAMMER: **************
--
-- NOTES:
-- A TCHAR* is passed to AppendText() to be appended to the output window
----------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:	AppendText
-- DATE:	    23/11/15
-- REVISIONS:   (V1.0)
-- DESIGNER:	Joseph Tam-Huang
-- PROGRAMMER:  Joseph Tam-Huang
-- INTERFACE:   void AppendText(const HWND &hwnd, TCHAR *message)
--			        const HWND &hwnd: the handle of the output window
--					TCHAR *message: the message to append
-- RETURN:		void
--
-- NOTES:
-- Appends the message to the existing text on the output window.
----------------------------------------------------------------------------------------------------------------------*/
void AppendText(const HWND &hwnd, TCHAR *message)
{
	// get the current selection
	DWORD StartPos, EndPos;
	SendMessage(hwnd, EM_GETSEL, reinterpret_cast<WPARAM>(&StartPos), reinterpret_cast<WPARAM>(&EndPos));

	// move the caret to the end of the text
	int outLength = GetWindowTextLength(hwnd);
	SendMessage(hwnd, EM_SETSEL, outLength, outLength);

	// insert the text at the new caret position
	SendMessage(hwnd, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(message));

	// restore the previous selection
	SendMessage(hwnd, EM_SETSEL, StartPos, EndPos);
}

HWND CreateListView(const HWND &hwndParent)
{
	RECT rcClient;
	GetClientRect(hwndParent, &rcClient);
	size_t listViewWidth = SIZE_BTN_ROW - 20;

	HWND hListView = CreateWindowEx(
		0,
		WC_LISTVIEW,
		NULL,
		WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_SHOWSELALWAYS | LVS_REPORT,
		10, 10,
		listViewWidth,
		LISTVIEW_HEIGHT,
		hwndParent, NULL, NULL, NULL);

	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = COL_1;
	lvc.pszText = "#";
	lvc.cx = listViewWidth / 3;
	lvc.fmt = LVCFMT_LEFT;
	ListView_InsertColumn(hListView, 0, &lvc);

	lvc.iSubItem = 0;
	lvc.pszText = "STATISTICS";
	lvc.cx = listViewWidth * 2 / 3;
	ListView_InsertColumn(hListView, 0, &lvc);

	LVITEM lvI;
	lvI.mask = LVIF_TEXT;
	lvI.iSubItem = COL_0;
	lvI.pszText = "Packets Sent";
	lvI.iItem = 0;
	ListView_InsertItem(hListView, &lvI);

	lvI.pszText = "Packets Received";
	lvI.iItem = 1;
	ListView_InsertItem(hListView, &lvI);

	lvI.pszText = "Packets Dropped";
	lvI.iItem = 2;
	ListView_InsertItem(hListView, &lvI);

	return hListView;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:	updateStatistic
-- DATE:	    02/12/15
-- REVISIONS:   (V1.0)
-- DESIGNER:	Joseph Tam-Huang
-- PROGRAMMER:  Joseph Tam-Huang
-- INTERFACE:   void updateStatistic(HWND hList, int sent, int rcv, int drop)
--					HWND hList
-- RETURN:		HANDLE : the handle of the file created for writting
--
-- NOTES:
-- Creates the file for storing the text ouput.
----------------------------------------------------------------------------------------------------------------------*/
void updateStatistic(HWND hList, int sent, int rcv, int drop) {
	char s[10];
	char r[10];
	char d[10];
	sprintf_s(s, "%d", sent);
	sprintf_s(r, "%d", rcv);
	sprintf_s(d, "%d", drop);
	ListView_SetItemText(hList, 0, COL_1, s);
	ListView_SetItemText(hList, 1, COL_1, r);
	ListView_SetItemText(hList, 2, COL_1, d);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:	CreateFileForWriting
-- DATE:	    02/12/15
-- REVISIONS:   (V1.0)
-- DESIGNER:	Joseph Tam-Huang
-- PROGRAMMER:  Joseph Tam-Huang
-- INTERFACE:   HANDLE CreateFileForWriting()
-- RETURN:		HANDLE : the handle of the file created for writting
--
-- NOTES:
-- Creates the file for storing the text ouput.
----------------------------------------------------------------------------------------------------------------------*/
HANDLE CreateFileForWriting() {
	HANDLE hFile;
	hFile = CreateFile("Output.txt",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (hFile == INVALID_HANDLE_VALUE) {
		OutputDebugString("failed to create file or already exists");
		return NULL;
	}
	return hFile;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:	writeToOutputFile
-- DATE:	    02/12/15
-- REVISIONS:   (V1.0)
-- DESIGNER:	Joseph Tam-Huang
-- PROGRAMMER:  Joseph Tam-Huang
-- INTERFACE:   void writeToOutputFile(HANDLE writeFile, char* message)
--			        const HAnDLE writeFile: the handle of the output file
--					char *message: the message to write to the file
-- RETURN:		void
--
-- NOTES:
-- Writes the payload of the received packet into a file.
----------------------------------------------------------------------------------------------------------------------*/
void writeToOutputFile(HANDLE writeFile, char* message) {
	DWORD dwWritten;
	if (writeFile != NULL) {
		SetFilePointer(writeFile, 0, NULL, FILE_END);
		WriteFile(writeFile, message, strlen(message), &dwWritten, NULL);
		OutputDebugString("wrote to file\n");
	}
	else {
		OutputDebugString("Failed to write to file\n");
	}

}
