#define _CRT_SECURE_NO_WARNINGS
#include <tchar.h>
#include <vector>
#include <string>
#include "windows.h"
#include "iostream"
using namespace std;

#define BUFSIZE 512

char data[BUFSIZE + 1] = "\0";

void fileRead(HANDLE mfile) {
	bool bString;
	OVERLAPPED ov;

	ov.Offset = ov.OffsetHigh = 0;
	ov.hEvent = NULL;
	bString = ReadFile(mfile, ::data, sizeof(::data), 0, &ov);

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
			return;
		}
	}
}

void fileWrite(HANDLE mfile) {
	bool bString;
	OVERLAPPED ov;
	DWORD size;

	ov.Offset = ov.OffsetHigh = 0;
	ov.hEvent = NULL;
	size = SetFilePointer(mfile, 0, 0, FILE_END);
	bString = WriteFile(mfile, ::data, size, 0, &ov);

	if (!bString) {
		if (GetLastError() == ERROR_IO_PENDING) {
			if (WaitForSingleObject(mfile, INFINITE) == WAIT_OBJECT_0) {
				if (ov.Internal == 0) {
					bString = true;
				}
			}
		}

		if (!bString) {
			cout << "WriteFile failed, code : " << GetLastError() << endl;
			return;
		}
	}
}

void print(HANDLE mfile) {
	DWORD dwFileSize = SetFilePointer(mfile, 0, 0, FILE_END);
	::data[dwFileSize] = '\0';
	cout << ::data << endl;
}

void reverse() {
	vector<string> str;
	int pos = 0;
	string token = "";
	string input(::data);

	while ((pos = input.find(" ")) != string::npos) {
		token = input.substr(0, pos);
		str.push_back(token);
		input.erase(0, pos + 1);
	}
	str.push_back(input);
	reverse(str.begin(), str.end());
	
	for (int i = 0; i < str.size(); i++) {
		//printf("%d¹øÂ° %s\n", i, str[i]);
		//sprintf(::data, "%s ", str[i]);
	}
	printf("%s", ::data);
}

void main() {
	HANDLE hFile = CreateFile
	(
		_T("./file.txt"),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL
	);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		cout << "CreateFile failed, code : " << GetLastError() << endl;
		return;
	}

	fileRead(hFile);
	reverse();
	fileWrite(hFile);
	fileRead(hFile);
	print(hFile);

	CloseHandle(hFile);
}