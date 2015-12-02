// Program WinMenu.cpp
//test
#define STRICT

#include <windows.h>
#include <stdio.h>
#include <string>
#include "source.h"
#include "events.h"
#include "codes.h"
#include <commctrl.h>
#include "checksum.h"
#include <chrono>
#include <deque>
#include <vector>


// need these at the top:
enum states { idle, waitPacket, waitAck, waitAck2, wait, sendState, receiveState , sendingPacket};
states status;

unsigned char depacketizedData[512];
DWORD WINAPI readThread(LPVOID hwnd);
DWORD WINAPI readInFileThread(LPVOID hwnd);
HANDLE wThread, rThread;
boolean idleReadEnq(DWORD time);
using namespace std;
void checkPriority(states cur);
static const int COMMAND_MODE = 1;
static const int READY_TO_CONNECT_MODE = 2;
static const int CONNECT_MODE = 3;
void checkStatus(BYTE type);
bool terrible = false;
bool dataToRead = false;
boolean checkForSoh();
BOOL writePacket(BYTE type); vector<unsigned char> dataBuffer;
void acknowledgeLine();
boolean inIdle = false;
DWORD wThreadId, rThreadId;
//char Name[] = "Radio Comm";
void sendEnq();
//LPCSTR lpszCommName = "COM1";
COMMCONFIG	cc;			//Communcation configurations
						//HANDLE		hComm;		//Handle for Serial Port
HANDLE		hThrd;		//Thread Handle for reading
int			Mode = COMMAND_MODE;	//Global variable to switch between modes.

char Name[] = TEXT("Radio Modem Protocol Driver");
int syncBit = 0;
bool inWrite = false;
deque<string> packetsOk;
unsigned char * packetize(const unsigned char * data);
unsigned char * depacketize(unsigned char * packet);
unsigned char getSyncBit();
DWORD WINAPI writeThread(LPVOID hwnd);
BOOL writeDataPacket(const unsigned char* data);
bool sendPriority = false;
void writePackets();
bool receievePriority = false;
char str[80] = "";

BOOL readInPacket();
bool timeoutWait(DWORD ms);
void acknowledgeEnq();
char * readBuffer;
vector<unsigned char>packets;
void checkSendPriority();
void checkReceivePriority();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void writingState();
//Handle for the output window
HWND hOutput;
//Handle for the list view
HWND hList; // will use for displaying statistics - Needs to be implemented
			//Handle for connect button
HWND hConnectToggleBtn;
//Handle for connect button
HWND hConnectBtn;
//Handle for the priority button
HWND hPriorityBtn;
//Handle for the display button
HWND hDisplayToggleBtn;
//Select file button
HWND hSelectFileBtn;
//HDC
HDC hdc;
//HWND
HWND hwnd;

#pragma warning (disable: 4096)

char	lpszCommName[] = TEXT("com1");
HANDLE hComm;

/*
=========================================================================
FUNCTION:		ConnectionRead

DATE:			September 26, 2015

REVISIONS:		September 27, 2015
First attempt of letting individual character read.
September 30, 2015
Changed visual colour to debug issue of delayed
character output.
October 3, 2015
Proper character read. Created the DisplayMessage()
function to handle character output to window.
October 4, 2015
Visual code clean-up, no major changes.

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER:		Tyler Trepanier-Bracken

INTERFACE:		DWORD WINAPI ConnectionRead(
LPVOID hwnd)

PARAMETERS:		LPVOID hwnd
Window to output charcters.

RETURNS:		DWORD WINAPI

NOTES:
This is a thread function that contains an infinite loop that
continously reads a pre-established and connected serial port seraching
for individual characters being received. This thread function is
overlapped and
=========================================================================
*/
DWORD WINAPI ConnectionRead(LPVOID); // prototype for a thread read

									 /*
									 =========================================================================
									 FUNCTION:		ConnectionWrite

									 DATE:			September 26, 2015

									 REVISIONS:		September 27, 2015
									 First attempt of letting individual character output.
									 September 30, 2015
									 Allowed for threading.
									 October 3, 2015
									 Removed threading for less processor usage and
									 simplicity.
									 October 4, 2015
									 Visual code clean-up, no major changes.

									 DESIGNER:		Tyler Trepanier-Bracken

									 PROGRAMMER:		Tyler Trepanier-Bracken

									 INTERFACE:		BOOL ConnectionWrite(
									 HWND hwnd,
									 WPARAM wParam)

									 PARAMETERS:		HWND hwnd
									 Window designed to be emptied of painted objects.

									 RETURNS:		TRUE
									 Successful character output to external device.
									 FALSE
									 Unsuccessful character output.


									 NOTES:
									 This function takes in a windows object and clears the screen using many
									 space characters for the entire size of the window.

									 Requires two definitions: MAX_HEIGHT and MAX_WIDTH, these are the
									 window's size.
									 =========================================================================
									 */
BOOL ConnectionWrite(HWND, BYTE[], size_t); // prototype for a thread write

											/*
											=========================================================================
											FUNCTION:		Open Port

											DATE:			September 26, 2015

											REVISIONS:		October 4, 2015
											Made

											DESIGNER:		Tyler Trepanier-Bracken

											PROGRAMMER:		Tyler Trepanier-Bracken

											INTERFACE:		BOOL OpenPort(
											HWND hwnd)

											PARAMETERS:		HWND hwnd
											Window where error messages will be printed on.

											RETURNS:		FALSE,
											Failed opening the port for overlapped reading
											and writing.
											TRUE
											Successful port opening for overlapped reading and
											writing.
											NOTES:
											Accepts a window (for displaying errors) and the combined port settings
											for opening a serial port for reading and writing.

											Returns failure if BuildCommDCB(), GetCommState(), SetCommState()
											or SetCommTimeouts() fails. Success overwise.
											=========================================================================
											*/
BOOL OpenPort(HWND);

/*
=========================================================================
FUNCTION:		Get Packet

DATE:			November 27, 2015

REVISIONS:

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER:		Tyler Trepanier-Bracken

INTERFACE:		char* getPacket(BYTE type)

PARAMETERS:		BYTE type
Decides what type of packet to grab

RETURNS:		char*,
Pointer to an char array that contains a ENQ, ACK, DC1 or DC2 packet
depending

NOTES:
Depending on the parameter that is passed through, the returned packet
will be accompanied with the associated parameter.
=========================================================================
*/
BYTE* getPacket(BYTE, BYTE[]);

bool timeoutWait(DWORD ms) {
	SetCommMask(hComm, EV_RXCHAR);
	DWORD dwCommEvent = 0;
	unsigned char buffer[2] = { 0 };
	OVERLAPPED	osReader = { 0 };
	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (WaitCommEvent(hComm, &dwCommEvent, &osReader)) {

	}
	else {
		//look into waitforsingleobject return value
		if (!ReadFile(hComm, &buffer[0], 1, &dwCommEvent, &osReader)) {
			if (GetLastError() == ERROR_IO_PENDING) {
				if (WaitForSingleObject(osReader.hEvent, ms))
					return false;
			}
		}
	}
	if (buffer[0] == ACK) {
		OutputDebugString("\nReceived ack");

		return true;
	}
	OutputDebugString("\nDid not recieve ACK, but antoher character or something");
	return false;

}
boolean checkForSoh(DWORD ms) {
	OutputDebugString("In idleReadEnq");
	SetCommMask(hComm, EV_RXCHAR);
	DWORD dwCommEvent = 0;
	unsigned char buffer[2] = { 0 };
	OVERLAPPED	osReader = { 0 };
	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (WaitCommEvent(hComm, &dwCommEvent, NULL)) {
		OutputDebugString("event received");
		if (dwCommEvent == 0) {
			OutputDebugString("\nExit Thread in SOH");
			ExitThread(0);
		}
		do {
			buffer[0] = 0x00;
			if (!ReadFile(hComm, &buffer[0], 1, &dwCommEvent, &osReader)) {
				if (GetLastError() == ERROR_IO_PENDING) {
					WaitForSingleObject(osReader.hEvent, ms);
				}

			}
			if (buffer[0] == ENQ) {
				OutputDebugString("\nHAD TO SEND BACK ACK OK");
				writePacket(ACK);
			}

		} while (buffer[0] != SOH);
		OutputDebugString("\nSOH RECEIVED OK");



	}


	return true;
}
void beIdle() {
	OutputDebugString("\nIn beIdle, the thread should die NOW");
		rThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&readThread, (LPVOID)hwnd,
			0, &rThreadId);
	
}

DWORD WINAPI readThread(LPVOID hwnd) {
	OutputDebugString("\nREAD THREAD STARTING");
	int attempts = 0;
	bool gotEnq = false;
	terrible = false;
	if (inWrite) {
		OutputDebugString("\nWas writing, now doing timeout");
		terrible = true;
		gotEnq = idleReadEnq((DWORD)5000);
		inWrite = false;
	}
	if (!gotEnq) {
		
		PurgeComm(hComm, PURGE_RXCLEAR);
		if (dataToRead) {
			if (packetsOk.size() == 1)
				dataToRead = false;
			OutputDebugString("\nStarting write thread");
			wThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&writeThread, (LPVOID)hwnd,
				0, &wThreadId);
			inWrite = true;
			OutputDebugString("\nKilling read thread in readThread");
			ExitThread;
			}
		else {
			OutputDebugString("Going to wait for ENQ, INFINITE");
		idleReadEnq(INFINITE);
		}
		
	
	}
		
	
	OutputDebugString("\nAknowledging ACK in read thread");
	acknowledgeEnq();

	while (!checkForSoh(READ_TIMEOUT)) {
		if (attempts++ == 4) {
			OutputDebugString("\nGiving up on reading SOH OK");
			break;
		}
	}
	OutputDebugString("\nReading in packet");
	readInPacket();
	OutputDebugString("\nRead packet DONE -- checking receive priority.");
	checkReceivePriority();
	OutputDebugString("\nREAD THREAD ENDING");
	return 0;
}
DWORD WINAPI writeThread(LPVOID hwnd) {
	OutputDebugString("\nWRITE THREAD STARTING");
	OutputDebugString("\nTop of WRITE, sending ENQ");
	sendEnq();
	while (!(timeoutWait(1500))) {

		writePacket(ENQ);
	}
	OutputDebugString("\nWriting a packet");
	writePackets();
	OutputDebugString("\nCheck send priority in writeThread");
	checkSendPriority();
	OutputDebugString("\nWRITE THREAD ENDING");
	return 0;
}
void checkSendPriority() {
	//Sleep(3000);
	beIdle();
}
void checkReceivePriority() {
	//Sleep(3000);
	beIdle();
}
void acknowledgeEnq() {
	writePacket(ACK);

}
void writePackets() {
	int attempts = 0;
	string temp = packetsOk.front();
	temp = temp.substr(0, temp.size() - 1);
	packetsOk.pop_front(); 
	const unsigned char * data = reinterpret_cast<const unsigned char *> (temp.c_str());

	//unsigned char data[] = "Hello, this is a test. I hope it works because I really dont like this and want to sleep all day...";
	
	writeDataPacket(data);
	while (!timeoutWait(1000)) {
		OutputDebugString("\nTimeoutWait in writePackets did not get ACK");
		writeDataPacket(data);
		if (attempts++ == 4) {
			OutputDebugString("Giving up on writing");
			break;
		}
	}
}
boolean idleReadEnq(DWORD time) {
	

	inIdle = true;
	SetCommMask(hComm, EV_RXCHAR);
	DWORD dwCommEvent = 0;
	unsigned char buffer[2] = { 0 };
	OVERLAPPED	osReader = { 0 };
	
	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (inWrite && !terrible){
		WaitCommEvent(hComm, &dwCommEvent, NULL);
		inWrite = false;
	}
	if (WaitCommEvent(hComm, &dwCommEvent, NULL)) {
		OutputDebugString("event received");
		if (dwCommEvent == 0) {
			inWrite = true;
			inIdle = false;
			OutputDebugString("\nKilling thread in idleReadEnq");
			ExitThread(0);
		}
		do {
			buffer[0] = 0x00;
			if (!ReadFile(hComm, &buffer[0], 1, &dwCommEvent, &osReader)) {
				if (GetLastError() == ERROR_IO_PENDING) {
					if (WaitForSingleObject(osReader.hEvent, time)) {
					
						inIdle = false;
						return false;
						
					}
					
				}

			}
			
		} 
		while (buffer[0] != ENQ);




	}
	inIdle = false;
	return true;
}

void sendEnq() {
	SetCommMask(hComm, EV_KILL_THREAD);
	writePacket(ENQ);

	//if (timeoutWait(500)) {
		//unsigned char data[] = "Successful transfer. This is a bigger string. MORE MORE MORE MORE MORE MORE MORE, OKAY.";
		//writingState();
	//}
	//else {
		//OutputDebugString("Timeout, no ack receieved");
	//}
}
BOOL readInPacket()
{
	int attempts = 0;
	BYTE		buffer[516] = { 0x00 };			//Byte array which will hold incoming input.
	unsigned char		readPacket[516] = { 0x00 };		//char array that will hold a packet read in
	unsigned int index = 1;						//dynamic index for the readPacket array.
	BOOL startPacket = false;
	BOOL		readComplete = false;			//Flag to check if a complete packet was read.
	BOOL		readingPacket = false;			//Flag if a packet was detected.
	BOOL		success = false;				//Boolean for read success.
	BOOL		waitRead = false;				//Flag to check if reading is incomplete

	DWORD		dwRead, dwCommEvent, dwRes;		//event checkers
	OVERLAPPED	osReader = { 0 };				//Contains information used in asynchronous 
	readPacket[0] = SOH;									//(or overlapped) input and output (I/O).

	
	if (!SetCommMask(hComm, EV_RXCHAR)) {
		OutputDebugString("Set comm mask failed");
	}
	while (Mode > COMMAND_MODE) {
		if (Mode == READY_TO_CONNECT_MODE)
			continue;

		osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (osReader.hEvent == NULL)
			continue;

		if (!WaitCommEvent(hComm, &dwCommEvent, &osReader)) {
			if (GetLastError() == ERROR_IO_PENDING) {
				do {
					if (!ReadFile(hComm, &buffer[0], 1, &dwCommEvent, &osReader)) {
						if (GetLastError() == ERROR_IO_PENDING) {
							WaitForSingleObject(osReader.hEvent, READ_WAIT);
						}
					}

					if (buffer[0] != 0x00) {
						/*OutputDebugString("[[");
						OutputDebugString((char*)buffer);
						OutputDebugString("]]\n");
						//buffer[0] = 0x00;*/
					}

					if (buffer[0] != 0x00) {
						readPacket[index++] = buffer[0];
					}

					//OutputDebugString((char*)readPacket);
					if (buffer[0] == EOT) {
						
						unsigned char * aPacket = depacketize(readPacket);
						if (aPacket != 0x00) {
							OutputDebugString("\nPacket received properly");
							writePacket(ACK);
							OutputDebugString("\nTHe packet is: ");
							OutputDebugString("\n");
							AppendText(hOutput, (TCHAR *)readPacket);
							CloseHandle(osReader.hEvent);
							writePacket(ACK);
							return true;

						}
						else {
							OutputDebugString("\nInvalid Packet");
							readPacket[index + 1] = 0x00;
							OutputDebugString((char *)readPacket);
							CloseHandle(osReader.hEvent);
							return false;

						}
					}




					buffer[0] = 0x00;
				} while (dwCommEvent);
			}
			else {
				if (WaitForSingleObject(osReader.hEvent, READ_TIMEOUT)) {
				
					return false;
				}

			}
		
			
				


		}
	}
	
	return 0;
}

/*bool timeoutWait(DWORD ms) {
	SetCommMask(hComm, EV_RXCHAR);
	DWORD dwCommEvent = 0;
	unsigned char buffer[2] = { 0 };
	OVERLAPPED	osReader = { 0 };
	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (WaitCommEvent(hComm, &dwCommEvent, &osReader)) {
		
	}
	else {
		//look into waitforsingleobject return value
		if (!ReadFile(hComm, &buffer[0], 1, &dwCommEvent, &osReader)) {
			if (GetLastError() == ERROR_IO_PENDING) {
				if(WaitForSingleObject(osReader.hEvent, ms))
					return false;
			}
		}
	}

	OutputDebugString("Received ack");
	return true;

}*/

BOOL OpenPort(HWND hwnd)
{

	OutputDebugString("Start Open Port\n");
	if (!GetCommState(hComm, &cc.dcb))
	{
		OutputDebugString("OpenPort: GetCommState failed\n");
		Mode = COMMAND_MODE;
		return FALSE;
	}

	/*if (!SetCommState(hComm, &cc.dcb))
	{
	OutputDebugString("OpenPort: SetCommState failed\n");
	Mode = COMMAND_MODE;
	return FALSE;
	}*/

	//Attempt to create the TimeOut for reading and writing purposes.
	/*COMMTIMEOUTS TimeOut;
	TimeOut.ReadIntervalTimeout = 3;
	TimeOut.ReadTotalTimeoutMultiplier = 3;
	TimeOut.ReadTotalTimeoutConstant = 2;
	TimeOut.WriteTotalTimeoutMultiplier = 3;
	TimeOut.WriteTotalTimeoutConstant = 2;

	if (!SetCommTimeouts(hComm, &TimeOut))
	{
	OutputDebugString("OpenPort: SetCommTimeouts failed\n");
	Mode = COMMAND_MODE;
	return FALSE;
	}*/

	return TRUE;
}



BOOL ConnectionWrite(HWND hwnd, BYTE message[], size_t size) // need this to point to a structure that I can use.
{

	OVERLAPPED	osWrite = { 0 };
	BOOL		writeSuccess;
	DWORD		dwWrite;
	size_t		index = 0;

	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent == NULL)
		return FALSE; //things went boom no good

					  //Temporary, will not need this line after changing parameters!!!
					  //sprintf_s(Input, "%c", (char)wParam);
	BYTE works[2] = { SOH, ACK };
	while (index < size) {
		writeSuccess = FALSE;
		//if(!WriteFile(hComm, newParam, dwToWrite, &dwWritten, &osWrite))
		if (!WriteFile(hComm, &(*(message + index)), 1, &dwWrite, &osWrite))
		{
			if (GetLastError() != ERROR_IO_PENDING) {
				writeSuccess = FALSE; // Write screwed up, GTFO
				return false;
			}
			else { //Write is still doing shit
				if (!GetOverlappedResult(hComm, &osWrite, &dwWrite, TRUE)) {
					writeSuccess = FALSE; //Write is pending, should try again I guess
				}
				else {
					writeSuccess = TRUE;
					index++;
					//OutputDebugString("Connection Write: writefile returned false\n");
				}
			}
		}
		//Writefile worked immediately
		else
		{
			writeSuccess = TRUE;
			index++;
			OutputDebugString("Connection Write: writefile returned true\n");
		}
	}

	OutputDebugString("Connection Write: Write thread ended\n");
	CloseHandle(osWrite.hEvent);

	return writeSuccess;
}

BYTE* getPacket(BYTE type, BYTE packet[]) {
	//BYTE packet[2] = { SOH };
	packet[0] = SOH;
	switch (type) {
	case SOH:
		OutputDebugString("Error, SOH detected in getPacket");
		break;
	case ENQ:
	case ACK:
		packet[1] = type;
		break;
	case DC1:	// <--- WHAT WAS THE PURPOSE OF THIS AGAIN, TYLER FORGOT
	case DC2:	// <--- WHAT WAS THE PURPOSE OF THIS AGAIN, TYLER FORGOT
		packet[1] = type;
		break;
	default:
		//something didn't go right, make sure it fucks up.
		packet[0] = packet[1] = 0x00;
		break;
	}
	return packet;

}

BOOL writePacket(BYTE type) {
	BYTE packet[1] = { type };
	//getPacket(type, packet);
	if (packet == NULL)
		return FALSE;
	return ConnectionWrite(hwnd, packet, 1);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow)
{
	//HWND hwnd;
	MSG Msg;
	WNDCLASSEX Wcl;

	Wcl.cbSize = sizeof(WNDCLASSEX);
	Wcl.style = CS_HREDRAW | CS_VREDRAW;
	Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION); // large icon 
	Wcl.hIconSm = NULL; // use small version of large icon
	Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);  // cursor style

	Wcl.lpfnWndProc = WndProc;
	Wcl.hInstance = hInst;
	Wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //white background
	Wcl.lpszClassName = Name;

	Wcl.lpszMenuName = TEXT("MYMENU"); // The menu Class
	Wcl.cbClsExtra = 0;      // no extra memory needed
	Wcl.cbWndExtra = 0;

	if (!RegisterClassEx(&Wcl))
		return 0;

	hwnd = CreateWindow(Name, Name, WS_OVERLAPPEDWINDOW, 10, 10,
		700, 500, NULL, NULL, hInst, NULL);

	// Creating the output window
	hOutput = CreateOutputWindow(hwnd);

	hList = CreateListView(hwnd);

	updateStatistic(hList, PKT_SENT, 56);
	updateStatistic(hList, PKT_RCV, 156);
	updateStatistic(hList, PKT_DROP, 256);

	hConnectToggleBtn = CreateWindow(
		"BUTTON",
		"Connect",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		10, 90, 90, 20,
		hwnd,
		(HMENU)IDM_CONNECT,
		(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
		NULL);
		
	hPriorityBtn = CreateWindow(
		"BUTTON",
		"Priority",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		110, 90, 90, 20,
		hwnd,
		(HMENU)IDM_PRIORITY,
		(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
		NULL);

	hDisplayToggleBtn = CreateWindow(
		"BUTTON",
		"Display On",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		10, 120, 90, 20,
		hwnd,
		(HMENU)IDM_DISPLAY_ON,
		(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
		NULL);

	hSelectFileBtn = CreateWindow(
		"BUTTON",
		"Select File",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		110, 120, 90, 20,
		hwnd,
		(HMENU)IDM_SELECT_FILE,
		(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
		NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:   CreateOutputWindow
-- DATE:	   23/11/15
-- REVISIONS:  (V1.0)
-- DESIGNER:   Joseph Tam-Huang
-- PROGRAMMER: Joseph Tam-Huang
-- INTERFACE:  HWND CreateOutputWindow(HWND hwndParent)
--			       HWND hwndParent: The handle to the parent window
-- RETURNS:    hOutputWindow : The handle to the output window
--
-- NOTES:
-- Creates a list view that is used to display text on the window
----------------------------------------------------------------------------------------------------------------------*/
HWND CreateOutputWindow(HWND hwndParent)
{
	RECT rcClient;
	GetClientRect(hwndParent, &rcClient);
	size_t listViewWidth = rcClient.right - rcClient.left - SIZE_BTN_ROW;

	HWND hOutputWindow = CreateWindow(
		TEXT("EDIT"),
		TEXT(""),
		WS_CHILD | WS_VISIBLE | ES_LEFT | ES_WANTRETURN |
		ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY |
		WS_VSCROLL,
		SIZE_BTN_ROW, 0, listViewWidth, rcClient.bottom - rcClient.top,
		hwnd,  // use WM_SIZE and MoveWindow() to size
		NULL, GetModuleHandle(NULL), NULL);

	return (hOutputWindow);
}






/*---------------------------------------------------------------------------------
--	FUNCTION: packetize
--
--	DATE:		Nov 25, 2015
--
--	REVISIONS:	Nov 25, 2015 -
--
--	DESIGNER:	Gabriella Cheung
--
--	PROGRAMMER:	Gabriella Cheung
--
--	INTERFACE:	char * packetize(char * data)
--
--	PARAMETER:	data - char array containing chars to be packetized
--
--	RETURNS:	- an empty string if data is empty or if data has more than 512 char
--				- a char array in "packet" form if data is valid
--
--	NOTES:
--	This function puts a char array with size up to 512 bytes into a "packet", which
--	is a char array comprising of the header (SoH, sync bit, and the checksum (2 bytes))
--	and the payload which is data that was passed in as the parameter. If data has less
--	than 512 bytes, then the EoT character is added after the data. The checksum
--	is calculated using the checksum.h supplied by Tom Tang.
--
---------------------------------------------------------------------------------*/
unsigned char * packetize(const unsigned char * data) {
	//int dataSize = sizeof(data);
	
	unsigned char * x = (unsigned char*)data;
	int dataSize = strlen((char*)x);

	if ((dataSize <= 0) || (dataSize > 512)) {
		return 0x00;
	}

	unsigned char * packet;
	int packetSize = dataSize + 4;

	if (dataSize < 512) {
		packetSize++; //need to add EOT
	}

	packet = new unsigned char[packetSize];

	packet[0] = SOH; //SOH

	packet[1] = getSyncBit();

	checksum *chk = new checksum();
	chk->clear();

	for (int i = 0; i < dataSize; i++) {
		chk->add(data[i]);
	}

	std::vector<char> checksum = chk->get();

	packet[2] = checksum[0];
	packet[3] = checksum[1];

	int i;
	for (i = 0; i < dataSize; i++) {
		packet[4 + i] = data[i];
	}
	if (dataSize < 512) {
		packet[4 + i] = EOT; //add EOT
	}
	return packet;
}

/*---------------------------------------------------------------------------------
--	FUNCTION: depacketize
--
--	DATE:		Nov 25, 2015
--
--	REVISIONS:	Nov 25, 2015 -
--
--	DESIGNER:	Gabriella Cheung
--
--	PROGRAMMER:	Gabriella Cheung
--
--	INTERFACE:	char * depacketize(char * packet)
--
--	PARAMETER:	packet - char array received from serial port
--
--	RETURNS:	- an empty string if packet is not in correct format:
--					- size is 0 or over 516 bytes
--					- if first byte is not SoH
--					- if second byte is not the correct sync bit
--					- if checksum is not 0
--				- a char array containing the data if packet is valid
--
--	NOTES:
--	This function takes the char array that was received from the serial port and
--	checks that there are no errors. The first byte has to be the SoH, the second
--	byte has to be the correct sync bit. The checksum bytes are added to the checksum
--	object, then the data is read into a char array and added to the checksum object
--	until a EoT character was read. After all the data was read, the checksum is
--	retrieved; if it is zero, the data char array is returned. The checksum
--	is calculated using the checksum.h supplied by Tom Tang.
--
---------------------------------------------------------------------------------*/
unsigned char * depacketize(unsigned char * packet) {

	unsigned char * x = (unsigned char*)packet;
	int packetSize = strlen((char*)x);

	if ((packetSize <= 0) || (packetSize > 516)) {
		return 0x00;
	}

	if (packet[0] != 0x01) {
		return 0x00;
	}

	if (packet[1] != getSyncBit()) {
		return 0x00;
	}

	checksum *chk = new checksum();
	chk->clear();

	// add checksum bytes
	chk->add(packet[2]);
	chk->add(packet[3]);

	// loop through all the char in packet and add them to checksum at the same time
	int i = 0;
	while (packet[i + 4] != 0x04) {
		depacketizedData[i] = packet[i + 4];
		chk->add(depacketizedData[i]);
		i++;
	}

	std::vector<char> checksum = chk->get();

	// if checksum does not return 0
	if (!(chk->check(checksum[0], checksum[1]))) {
		return 0x00;
	}

	return depacketizedData;
}

/*---------------------------------------------------------------------------------
--	FUNCTION: getSyncBit
--
--	DATE:		Nov 25, 2015
--
--	REVISIONS:	Nov 25, 2015 -
--
--	DESIGNER:	Gabriella Cheung
--
--	PROGRAMMER:	Gabriella Cheung
--
--	INTERFACE:	char getSyncBit()
--
--	RETURNS:	0x0F if sync bit is 0, 0xF0 if sync bit is 1
--				NULL if sync bit is neither 0 or 1
--
--	NOTES:
--	This function returns the char used to signify sync bit. The sync bit is a
--	global variable that alternates between 0 and 1.
--
---------------------------------------------------------------------------------*/
unsigned char getSyncBit() {
	if (syncBit == 0) {
		return SYNC0;
	}

	if (syncBit == 1) {
		return SYNC1;
	}

	return NULL;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:   resizeOutputWindow
-- DATE:	   23/11/15
-- REVISIONS:  (V1.0)
-- DESIGNER:   Joseph Tam-Huang
-- PROGRAMMER: Joseph Tam-Huang
--
-- INTERFACE:  void resizeOutputWindow()
-- RETURNS:    void
--
-- NOTES:
-- Resizes the output window to match the parent window
----------------------------------------------------------------------------------------------------------------------*/
void resizeOutputWindow() {
	RECT rcClient;
	GetClientRect(hwnd, &rcClient);
	size_t outputWindowWidth = rcClient.right - rcClient.left - SIZE_BTN_ROW;
	MoveWindow(
		hOutput,
		SIZE_BTN_ROW,
		0,
		outputWindowWidth,
		rcClient.bottom - rcClient.top,
		TRUE
	);
}



BOOL writeDataPacket(const unsigned char * data) {
	unsigned char * packet = packetize(data);
	size_t size = strlen((char *)data) < 512 ? strlen((char *)data) + 5 : strlen((char *)data) + 4;
	if (packet[0] == NULL) {
		return FALSE;
	}
	return ConnectionWrite(hwnd, (BYTE*)packet, size);
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:   selectFile
-- DATE:	   25/11/15
-- REVISIONS:  (V1.0)
-- DESIGNER:   Joseph Tam-Huang
-- PROGRAMMER: Joseph Tam-Huang
--
-- INTERFACE:  HANDLE selectFile()
-- RETURNS:    HANDLE : The handle to the file
--
-- NOTES:
-- Displays a dialog that prompts the user tp select a file to read from. Return a handle to the file if created
-- succesfully.
----------------------------------------------------------------------------------------------------------------------*/
HANDLE selectFile() {
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name
	HANDLE hf;              // file handle
	HANDLE readInFile;
					// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn) == TRUE) {
		OutputDebugString(ofn.lpstrFile);
		hf = CreateFile(ofn.lpstrFile,
			GENERIC_READ,
			0,
			(LPSECURITY_ATTRIBUTES)NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			(HANDLE)NULL);
		readInFile = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&readInFileThread, (LPVOID)hf,
			0, &wThreadId);
		return hf;
	}
	return NULL;
}
DWORD WINAPI readInFileThread(LPVOID hwnd){
	int index = 0;
	OutputDebugString("In readFileThread");
	HANDLE hf = (HANDLE)hwnd;
	DWORD dwBytesRead = 0;
	const int MAX = 512;
	
	bool read;
	while (1){
		char readBuffer[MAX];
		index = 0;
		OutputDebugString("\nA new sentence");
		vector<unsigned char> ok;
		read = ReadFile(hf,
			readBuffer,
			MAX - 1,
			&dwBytesRead,
			0);
	//	OutputDebugString(readBuffer);
		OutputDebugString("\n");
		if (dwBytesRead == 511) {
			readBuffer[511] = 0;
			string s = readBuffer;
			packetsOk.push_back(s);
			OutputDebugString("This is 512");
			/*while (index < dwBytesRead)
			{
				
				packets.push_back(readBuffer[index++]);
			
			}*/
	
		}
		else if (dwBytesRead > 0){
			string s = readBuffer;
			readBuffer[dwBytesRead] = 0;
			packetsOk.push_back(s);
			/*while (index < dwBytesRead){
				ok.push_back(readBuffer[index++]);
				}*/
			if (inIdle){
				OutputDebugString("\n\nWas in idle, pushing stuff now\n\n");
				dataToRead = true;
				wThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&writeThread, (LPVOID)hwnd,
					0, &wThreadId);
			}else{
				dataToRead = true;
			}
			OutputDebugString("\nLast packet");
			return 0;
		}
		else if (read && dwBytesRead == 0){
			OutputDebugString("\nI dJosheph knewont know what this check was for");
			if (packetsOk.size() != 0){
				dataToRead = true;
			}
			if (inIdle){
				
				wThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&writeThread, (LPVOID)hwnd,
					0, &wThreadId);
			}
			return 0;
		}
	}

}
/*void writingState() {
	
	int attempts = 0;
	
	unsigned char* data = packets.front();
	packets.pop_front();
	OutputDebugString("\nThe packet is: ");
	OutputDebugString((char *)data);
	//unsigned char data[] = "Successful transfer. This is a bigger string. MORE MORE MORE MORE MORE MORE MORE, OKAY.Successful transfer. This is a bigger string. MORE MORE MORE MORE MORE MORE MORE, OKAY.Successful transfer. This is a bigger string. MORE MORE MORE MORE MORE MORE MORE, OKAY.Successful transfer. This is a bigger string. MORE MORE MORE MORE MORE MORE MORE, OKAY.Successful transfer. This is a bigger string. MORE MORE MORE MORE MORE MORE MORE, OKAY.&&&&&";
	while (attempts++ < 5) {
		writeDataPacket(data);
		if (timeoutWait(100)) {
			OutputDebugString("Ack returned back to sender");
			break;
		}
		else {
			OutputDebugString("Ack not received");
		}
	}
}*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
	WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	DWORD			ReadInput;

	switch (Message)
	{
	case WM_SIZE:
		resizeOutputWindow();
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_CONNECT:
			status = idle;
			Mode = COMMAND_MODE;
			SetFocus(hwnd);
			// Do stuff when "Connect" is selected from the menu
			OutputDebugString("Connect button or menu item pressed\n");

			if ((hComm = CreateFile(lpszCommName, GENERIC_READ | GENERIC_WRITE, 0,
				NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL))
				== INVALID_HANDLE_VALUE)
			{
				MessageBox(NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
				return FALSE;
			}

			//Set connection settings
			DCB state;
			ZeroMemory(&state, sizeof(DCB));
			if (GetCommState(hComm, &state))
			{
				state.BaudRate = CBR_9600;
				state.ByteSize = 8;
				state.Parity = 0;
				state.StopBits = 0;
				SetCommState(hComm, &state);
				Mode = READY_TO_CONNECT_MODE;
			}
			if (!OpenPort(hwnd)) {
				//Restart in CommandMode and allow user to reconfigure again.
				Mode = COMMAND_MODE;
				break;
			}

			//This creates the read thread (continuous loop that reads input)
			/*hThrd = CreateThread(NULL, 0, ConnectionRead, (LPVOID)hwnd,
				CREATE_SUSPENDED, &ReadInput);
			SetThreadPriority(hThrd, THREAD_PRIORITY_HIGHEST);
			ResumeThread(hThrd);
			*/
			beIdle();
			/*hThrd = CreateThread(NULL, 0, readThread, (LPVOID)hwnd,
				CREATE_SUSPENDED, &ReadInput);
			SetThreadPriority(hThrd, THREAD_PRIORITY_HIGHEST);
			ResumeThread(hThrd);*/
			//Allow for CONNECTION_MODE
			Mode = CONNECT_MODE;

			SetWindowText(hConnectToggleBtn, DISCONNECT);
			SetWindowLongPtr(hConnectToggleBtn, GWLP_ID, static_cast<LONG_PTR>(static_cast<DWORD_PTR>(IDM_DISCONNECT)));
			EnableMenuItem(GetMenu(hwnd), IDM_CONNECT, MF_GRAYED);
			EnableMenuItem(GetMenu(hwnd), IDM_DISCONNECT, MF_ENABLED);
			OutputDebugString("disconnect btn or menu item clicked\n");
			break;


			break;
		case IDM_DISCONNECT:
			// Do stuff when "Disconnect" is selected from the menu/btn click
			// if succesful disconnection do the following:
			SetWindowText(hConnectToggleBtn, CONNECT);
			SetWindowLongPtr(hConnectToggleBtn, GWLP_ID, static_cast<LONG_PTR>(static_cast<DWORD_PTR>(IDM_CONNECT)));
			EnableMenuItem(GetMenu(hwnd), IDM_DISCONNECT, MF_GRAYED);
			EnableMenuItem(GetMenu(hwnd), IDM_CONNECT, MF_ENABLED);
			OutputDebugString("disconnect btn or menu item clicked\n");
			break;
		case IDM_PRIORITY:
			// Do stuff when "Priority" is selected from the menu
			OutputDebugString("Priority button or menu item pressed\n");
			break;
		case IDM_DISPLAY_ON:
			// Allow messages to be printed onto the output window
			SetWindowText(hDisplayToggleBtn, DISPLAY_OFF);
			SetWindowLongPtr(hDisplayToggleBtn, GWLP_ID, static_cast<LONG_PTR>(static_cast<DWORD_PTR>(IDM_DISPLAY_OFF)));
			OutputDebugString("display on btn or menu item clicked\n");
			break;
		case IDM_DISPLAY_OFF:
			// Disallow messages to be printed onto the output window
			SetWindowText(hDisplayToggleBtn, DISPLAY_ON);
			SetWindowLongPtr(hDisplayToggleBtn, GWLP_ID, static_cast<LONG_PTR>(static_cast<DWORD_PTR>(IDM_DISPLAY_ON)));
			OutputDebugString("display off btn or menu item clicked\n");
			break;
		case IDM_SELECT_FILE:
			selectFile();
			OutputDebugString("Select File pressed\n");
			break;
		case IDM_EXIT:
			OutputDebugString("Exit menu item pressed\n");
			CloseHandle(hComm);
			CloseHandle(hThrd);
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_CHAR:
		hdc = GetDC(hwnd);     // get device context
		switch (Mode)
		{
		case COMMAND_MODE:
			OutputDebugString("Tried to press a key when in command mode.\n");
			break;

		case READY_TO_CONNECT_MODE:
			//Connected but no output will be given.
			OutputDebugString("Tried to press a key when it is not a secured connection.\n");
			break;

		case CONNECT_MODE:
			//Write a character to the external device using a keyboard.
			OutputDebugString("Pressed a key when properly connected.\n");

			char temp[516];
			sprintf_s(temp, "%c", TEXT(wParam));
			if ((char)wParam == 'e') {
				//sendEnq();
				wThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&writeThread, (LPVOID)hwnd,
					0, &wThreadId);
				
			}
			else if ((char)wParam == 'a') {
				OutputDebugString("Sending an ACK\n");
				writePacket(ACK);
				status = waitAck;
			}
			else if ((char)wParam == '1') {
				OutputDebugString("Sending an DC1\n");
				writePacket(DC1);
			}
			else if ((char)wParam == '2') {
				OutputDebugString("Sending an DC2\n");
				writePacket(DC2);
			}
			else {
				OutputDebugString("Sending Data\n");
				unsigned char data[] = "Successful transfer. This is a bigger string. MORE MORE MORE MORE MORE MORE MORE, OKAY.";
				writeDataPacket(data);
				OutputDebugString("Done sending data\n");
			}

			//ConnectionWrite(hwnd, (char*)temp);
			break;
		default:
			OutputDebugString("Mode screwed up somehow in pressed key. Mode=" + Mode);
			break;
		}
		break;
	case WM_DESTROY:	// Terminate program
		OutputDebugString("Destroyed Mode=" + Mode);
		CloseHandle(hComm);
		CloseHandle(hThrd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}
/*
void splitFile(HANDLE file) {
attempts = 0;
OutputDebugString("In splitFile");
DWORD dwBytesRead = 0;
const int MAX = 512;
bool read;
char readBuffer[MAX] = { 0 };
read = ReadFile(file,
readBuffer,
MAX - 1,
&dwBytesRead,
0);
OutputDebugString(readBuffer);
if (dwBytesRead == 511) {
OutputDebugString("This is 512");
}
if (read && dwBytesRead == 0)
{
stillWriting = false;

}
else {
//char * packet = packetize(readBuffer);
}


}
*/