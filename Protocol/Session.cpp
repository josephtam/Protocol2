#define STRICT
#include <windows.h>
#include "resource.h"

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