#define _CRT_SECURE_NO_WARNINGS
#include <tchar.h>
#include <string>
#include "windows.h"
#include "iostream"
using namespace std;

#define BUFSIZE 512

DWORD WINAPI ThreadProc(LPVOID lpParam);

void main() {
	const TCHAR* fileNames[] = {
		_T("./file1.txt"),
		_T("./file2.txt"),
		_T("./file3.txt"),
		_T("./file5.txt")
	};
	HANDLE hFile[4];
	HANDLE hThread[2];
	DWORD dwThreadID[2];

	for (int i = 0; i < 4; i++) {
		hFile[i] = CreateFile
		(
			fileNames[i], GENERIC_READ, 0, NULL,
			OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL
		);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			cout << "CreateFile open failed, code : " << GetLastError() << endl;
			return;
		}
	}

	hThread[0] =
		CreateThread(
			NULL, 0,
			ThreadProc,
			(LPVOID)(&hFile[0]),
			0, &dwThreadID[0]
		);

	hThread[1] =
		CreateThread(
			NULL, 0,
			ThreadProc,
			(LPVOID)(&hFile[2]),
			0, &dwThreadID[1]
		);

	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

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

	HANDLE * mfile = (HANDLE *)lpParam;

	ov.Offset = ov.OffsetHigh = 0;
	ov.hEvent = NULL;
	bString = ReadFile(mfile, data, sizeof(data), 0, &ov);

	if (!bString) {
		if (GetLastError() == ERROR_IO_PENDING) {
			if (WaitForSingleObject(mfile, INFINITE) == WAIT_OBJECT_0) {
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

	printf("%s\n", data);
}