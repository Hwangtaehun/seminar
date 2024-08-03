#define _CRT_SECURE_NO_WARNINGS
#include <tchar.h>
#include <string>
#include "windows.h"
#include "iostream"
using namespace std;

#define BUFSIZE 512

struct Filedata {
	HANDLE hIocp;
	HANDLE one;
	HANDLE two;
};

DWORD WINAPI ThreadProc(LPVOID lpParam);

void main() {
	const TCHAR* fileNames[] = {
		_T("./file1.txt"),
		_T("./file2.txt"),
		_T("./file3.txt"),
		_T("./file4.txt")
	};
	HANDLE hFile[4];
	HANDLE hThread[2];
	DWORD dwThreadID[2];
	Filedata array[2];

	for (int i = 0; i < 4; i++) {
		hFile[i] = CreateFile
		(
			fileNames[i], 
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			NULL
		);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			cout << "CreateFile open failed, code : " << GetLastError() << endl;
			return;
		}
	}

	array[0].one = hFile[0];
	array[0].two = hFile[2];
	array[1].one = hFile[1];
	array[1].two = hFile[3];
	array[0].hIocp = array[1].hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 2);

	hThread[0] =
		CreateThread(
			NULL, 0,
			ThreadProc,
			(LPVOID)(&array[0]),
			0, &dwThreadID[0]
		);

	hThread[1] =
		CreateThread(
			NULL, 0,
			ThreadProc,
			(LPVOID)(&array[1]),
			0, &dwThreadID[1]
		);

	if (hThread[0] == NULL || hThread[1] == NULL)
	{
		_tprintf(_T("Thread creation fault! \n"));
		return;
	}

	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

	for (int i = 0; i < 2; i++)
	{
		CloseHandle(array[i].hIocp);
	}

	for (int i = 0; i < 2; i++) {
		CloseHandle(hThread[i]);
	}

	for (int i = 0; i < 4; i++) {
		CloseHandle(hFile[i]);
	}
}

DWORD WINAPI ThreadProc(LPVOID lpParam) {
	bool bString;
	OVERLAPPED ov;
	char data[BUFSIZE + 1];

	Filedata* mfile = (Filedata *)lpParam;

	ov.Offset = ov.OffsetHigh = 0;
	ov.hEvent = NULL;
	bString = ReadFile(mfile->one, data, sizeof(data), 0, &ov);

	if (!bString) {
		if (GetLastError() == ERROR_IO_PENDING) {
			if (WaitForSingleObject(mfile->one, INFINITE) == WAIT_OBJECT_0) {
				if (ov.Internal == 0) {
					bString = true;
				}
			}
		}

		if (!bString) {
			cout << "ReadFile failed, code : " << GetLastError() << endl;
			return 0;
		}
	}

	data[ov.InternalHigh] = '\0';
	printf("%s\n", data);

	ov.Offset = ov.OffsetHigh = 0;
	ov.hEvent = NULL;
	bString = ReadFile(mfile->two, data, sizeof(data), 0, &ov);

	if (!bString) {
		if (GetLastError() == ERROR_IO_PENDING) {
			if (WaitForSingleObject(mfile->two, INFINITE) == WAIT_OBJECT_0) {
				if (ov.Internal == 0) {
					bString = true;
				}
			}
		}

		if (!bString) {
			cout << "ReadFile failed, code : " << GetLastError() << endl;
			return -1;
		}
	}

	data[ov.InternalHigh] = '\0';
	printf("%s\n", data);

	return 0;
}