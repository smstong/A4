#include <stdio.h>
#include <Windows.h>
#include <process.h>

const wchar_t* strEventMainExit = L"Event_MainExit";
const wchar_t* strEventP2Start = L"Event_P2Start";
const wchar_t* strEventP3Start = L"Event_P3Start";

void ThreadProc(void* state);

int flag = 1;
int main(int argc, char** argv)
{
	HANDLE hEventStart = CreateEvent(NULL, TRUE, FALSE, strEventP3Start);
	printf("put any key to start the game\n");
	getch();
	SetEvent(hEventStart);

	_beginthread(ThreadProc, 0, NULL);


	while (flag) {
		Sleep(1000);
		printf("I am alive p3\n");
	}
	printf("dieing...\n");
	Sleep(1000);
	return 0;
}

void ThreadProc(void* state)
{
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, strEventMainExit);
	WaitForSingleObject(hEvent, INFINITE);
	flag = 0;
	printf("Got event\n");

}