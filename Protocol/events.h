#pragma once

#include <windows.h>
#include <stdio.h>
#include <string>
/*
=========================================================================
FUNCTION:		Get Write Event (TEMPORARY VERSION)

DATE:			November 20, 2015

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER:		Tyler Trepanier-Bracken

REVISIONS:		November 20, 2015 (Tyler Trepanier-Bracken)
Takes a HWND parameter for debugging purposes.

INTERFACE:		HANDLE GetWriteEvent(	HWND	hwnd,
WPARAM	wParam)

PARAMETERS:
HWND	hwnd,
Window that will display a successful write event
creation.
WPARAM	wParam,
Character to be outputed to the display window.

RETURNS:		HANDLE
The handle to the Write Event itself that is
produced as a result.

NOTES:
This function produces an event which will be later caught by the windows
thread later on. Takes in a window and character object only to prove
success by displaying it to the window. Also returns the handle to the
event created.
=========================================================================
*/
HANDLE GetWriteEvent(HWND, WPARAM);

/*
=========================================================================
FUNCTION:		Create Write Event (TEMPORARY VERSION)

DATE:			November 20, 2015

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER:		Tyler Trepanier-Bracken

REVISIONS:		November 20, 2015 (Tyler Trepanier-Bracken)
Takes a HWND parameter for debugging purposes.

INTERFACE:		HANDLE GetWriteEvent(	HWND	hwnd,
WPARAM	wParam)

PARAMETERS:
HWND	hwnd,
Window that will display a successful write event
creation.
WPARAM	wParam,
Character to be outputed to the display window.

RETURNS:		BOOL
indicate eithers success or failure
to create a write event.

NOTES:
Puts a write event in the windows events queue.
=========================================================================
*/
BOOL CreateWriteEvent(HWND, WPARAM);


/*
=========================================================================
FUNCTION:		Get Read Event (TEMPORARY VERSION)

DATE:			November 20, 2015

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER:		Tyler Trepanier-Bracken

REVISIONS:		November 20, 2015 (Tyler Trepanier-Bracken)
Takes a HWND parameter for debugging purposes.

INTERFACE:		HANDLE GetReadEvent(HWND	hwnd,
WPARAM	wParam)

PARAMETERS:
HWND	hwnd,
Window that will display a successful read event
creation.
WPARAM	wParam,
Character to be outputed to the display window.

RETURNS:		HANDLE
The handle to the Read Event itself that is
produced as a result.

NOTES:
This function produces an event which will be later caught by the windows
thread later on. Takes in a window and character object only to prove
success by displaying it to the window. Also returns the handle to the
event created.
=========================================================================
*/
HANDLE GetReadEvent(HWND, WPARAM);

/*
=========================================================================
FUNCTION:		Create Read Event (TEMPORARY VERSION)

DATE:			November 20, 2015

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER:		Tyler Trepanier-Bracken

REVISIONS:		November 20, 2015 (Tyler Trepanier-Bracken)
Takes a HWND parameter for debugging purposes.

INTERFACE:		BOOL CreateReadEvent(	HWND	hwnd,
WPARAM	wParam)

PARAMETERS:
HWND	hwnd,
Window that will display a successful read event
creation.
WPARAM	wParam,
Character to be outputed to the display window.

RETURNS:		BOOL
indicate eithers success or failure
to create a read event.

NOTES:
Puts a read event in the windows events queue.
=========================================================================
*/
BOOL CreateReadEvent(HWND, WPARAM);

/*
=========================================================================
FUNCTION:		Get ACK Event

DATE:			November 27, 2015

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER:		Tyler Trepanier-Bracken

REVISIONS:		November 20, 2015 (Tyler Trepanier-Bracken)
Takes a HWND parameter for debugging purposes.

INTERFACE:		HANDLE GetReadEvent(HWND	hwnd)

PARAMETERS:
HWND	hwnd,
Window that will display a successful read event
creation.

RETURNS:		HANDLE
The handle to the ACK Event itself that is
produced as a result.

NOTES:
This function produces an event which will be later caught by the windows
thread later on. Takes in a window to prove success by displaying it to 
the window. Also returns the handle to the ACK event created.
=========================================================================
*/
HANDLE GetAckEvent(HWND hwnd);

/*
=========================================================================
FUNCTION:		Create ACK Event

DATE:			November 27, 2015

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER:		Tyler Trepanier-Bracken

REVISIONS:		November 27, 2015 (Tyler Trepanier-Bracken)
Takes a HWND parameter for debugging purposes.

INTERFACE:		BOOL CreateAckEvent(HWND hwnd)

PARAMETERS:
HWND	hwnd,
Window that will display a successful ACK event
creation.

RETURNS:		BOOL
indicate eithers success or failure
to create a ACK event.

NOTES:
Puts a ACK event in the windows events queue.
=========================================================================
*/
BOOL CreateAckEvent(HWND hwnd);

/*
=========================================================================
FUNCTION:		Get ENQ Event

DATE:			November 27, 2015

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER:		Tyler Trepanier-Bracken

REVISIONS:		November 27, 2015 (Tyler Trepanier-Bracken)
Takes a HWND parameter for debugging purposes.

INTERFACE:		HANDLE GetReadEvent(HWND hwnd)

PARAMETERS:
HWND	hwnd,
Window that will display a successful read event
creation.

RETURNS:		HANDLE
The handle to the Read Event itself that is
produced as a result.

NOTES:
This function produces an ENQ event which will be later caught by the windows
thread later on. Takes in a window to prove success by displaying it to 
the window. Also returns the handle to the event created.
=========================================================================
*/
HANDLE GetEnqEvent(HWND hwnd);

/*
=========================================================================
FUNCTION:		Create ENQ Event

DATE:			November 20, 2015

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER:		Tyler Trepanier-Bracken

REVISIONS:		November 20, 2015 (Tyler Trepanier-Bracken)
Takes a HWND parameter for debugging purposes.

INTERFACE:		BOOL CreateEnqEvent(HWND hwnd)

PARAMETERS:
HWND	hwnd,
Window that will display a successful read event
creation.
WPARAM	wParam,
Character to be outputed to the display window.

RETURNS:		BOOL
indicate eithers success or failure
to create a ENQ event.

NOTES:
Puts a ENQ event in the windows events queue.
=========================================================================
*/
BOOL CreateEnqEvent(HWND hwnd);

HANDLE GetAckEvent(HWND hwnd) {
	HANDLE AckEvent;
	AckEvent = CreateEvent(NULL,
		TRUE,
		FALSE,
		TEXT("AckEvent"));
	if (AckEvent == NULL) {
		return NULL;
	}
	return AckEvent;
}

BOOL CreateAckEvent(HWND hwnd) {
	HANDLE AckEvent = GetAckEvent(hwnd);
	if (!SetEvent(AckEvent)) {
		return false;
	}
	return true;
}

HANDLE GetEnqEvent(HWND hwnd) {
	HANDLE EnqEvent;
	EnqEvent = CreateEvent(NULL,
		TRUE,
		FALSE,
		TEXT("EnqEvent"));
	if (EnqEvent == NULL) {
		return NULL;
	}
	return EnqEvent;
}

BOOL CreateEnqEvent(HWND hwnd) {
	HANDLE EnqEvent = GetEnqEvent(hwnd);
	if (!SetEvent(EnqEvent)) {
		return false;
	}
	return true;
}

HANDLE GetWriteEvent(HWND hwnd, WPARAM wParam) {
	HANDLE WriteEvent;
	WriteEvent = CreateEvent(NULL,
		TRUE,
		FALSE,
		TEXT("WriteEvent"));
	if (WriteEvent == NULL) {
		//DisplayMessage(hwnd, "Write Event creation failed!");
		return NULL;
	}
	else {
		//DisplayMessage(hwnd, "Successful event creation");
	}
	return WriteEvent;
}

BOOL CreateWriteEvent(HWND hwnd, WPARAM wParam) {
	HANDLE writeEvent = GetWriteEvent(hwnd, wParam);
	if (!SetEvent(writeEvent)) {
		//DisplayMessage(hwnd, "Write Event was not set.");
		return false;
	}
	return true;
}

HANDLE GetReadEvent(HWND hwnd) {
	HANDLE ReadEvent;
	ReadEvent = CreateEvent(NULL,
		TRUE,
		FALSE,
		TEXT("ReadEvent"));
	if (ReadEvent == NULL) {
		//DisplayMessage(hwnd, "Write Event creation failed!");
		return NULL;
	}
	else {
		//DisplayMessage(hwnd, "Successful read event creation");
	}
	return ReadEvent;
}

BOOL CreateReadEvent(HWND hwnd) {
	HANDLE readEvent = GetReadEvent(hwnd);
	if (!SetEvent(readEvent)) {
		return false;
	}
	return true;
}

HANDLE GetReadTimeoutEvent(HWND hwnd) {
	HANDLE ReadTimoutEvent;
	ReadTimoutEvent = CreateEvent(	NULL,
									TRUE,
									FALSE,
									TEXT("ReadTimeoutEvent"));
	
	if (ReadTimoutEvent == NULL) {
		return NULL;
	}
	return ReadTimoutEvent;
}

BOOL CreateReadTimeoutEvent(HWND hwnd) {
	HANDLE ReadTimoutEvent = GetReadTimeoutEvent(hwnd);
	if (!SetEvent(ReadTimoutEvent)) {
		return false;
	}
	return true;
}