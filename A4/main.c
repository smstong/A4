/******************************************************************
Note:
	- GUI thread shouldn't call WaitForxxx()
	- OpenEvent() can be replaced by CreateEvent()
	- 

********************************************************************/

#define _CRT_SECURE_NO_WARNINGS

#include <process.h>
#include <Windows.h>
#include <stdio.h>
#include <wchar.h>

//const wchar_t* MutexName = L"MainExit";
const wchar_t* strEventMainExit = L"Event_MainExit";
const wchar_t* strEventP2Start = L"Event_P2Start";
const wchar_t* strEventP3Start = L"Event_P3Start";

int isExit = 0;

void ThreadProc_Race(void* state);
void MessageBoxF(TCHAR* szCaption, TCHAR* formt, ...);
typedef struct _Car {
	int id;
	HANDLE threadHandle;
	int pos;
}Car;

CRITICAL_SECTION cs;
Car cars[5];

int main(int argc, char** argv)
{
	InitializeCriticalSection(&cs);

	HANDLE hEventGameExit = CreateEvent(NULL, TRUE, FALSE, strEventMainExit);
	HANDLE P2P3[] = { NULL, NULL };

	P2P3[0] = CreateEvent(NULL, FALSE, FALSE, strEventP2Start);
	P2P3[1] = CreateEvent(NULL, FALSE, FALSE, strEventP3Start);

	printf("I am waiting start from P2 or P3\n");

	SetEvent(P2P3[0]);  // For test only

	DWORD eventIndex = WaitForMultipleObjects(2, P2P3, FALSE, INFINITE) - WAIT_OBJECT_0;
	switch (eventIndex)
	{
	case 0:
		printf("I am started by P2\n");
		break;
	case 1:
		printf("I am started by P3\n");
		break;
	default:
		break;
	}


	int i;
	for (i = 0; i < sizeof(cars) / sizeof(cars[0]); i++) {
		cars[i].pos = 0;
		cars[i].id = i;
	}
	for (i = 0; i < sizeof(cars) / sizeof(cars[0]); i++) {
		cars[i].threadHandle = _beginthread(ThreadProc_Race, 0, (void*)(&cars[i]));
	}

	while (!isExit) {
		printf("I am running now...\n");

		// inform p2 and p3 to exit
		printf("press any key to exit and inform others\n");
		getch();
		SetEvent(hEventGameExit); 
		break;
	}

	DeleteCriticalSection(&cs);
	return 0;
}

int WhoWin()
{
	int i = 0;
	int winner = -1;
	EnterCriticalSection(&cs);
	for (i = 0; i < 5; i++) {
		if (cars[i].pos >= 10) {
			MessageBoxF(L"Winner", L"Winner is %d\n", cars[i].id);
			winner = i;
			break;
		}
		Sleep(100);
	}
	LeaveCriticalSection(&cs);
	return winner;
}
void ThreadProc_Race(void* state)
{
	Car* car = (Car*)state;
	int winner;
	while ((winner = WhoWin()) < 0) {
		car->pos += 1;
		printf("%d running: %d\n", car->id, car->pos);
		Sleep(100);
	}
	printf("%d won. I quit.\n", winner);
}
void MessageBoxF(TCHAR* szCaption, TCHAR* format, ...)
{
	TCHAR szBuf[1024];
	va_list args;

	va_start(args, format);
	_vsnwprintf(szBuf, sizeof(szBuf) / sizeof(TCHAR), format, args);
	va_end(args);

	MessageBox(NULL, szBuf, szCaption, 0);
}
