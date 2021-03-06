
#include <Windows.h> // for win32 functions
#include <tlhelp32.h>
#include <stdio.h>


struct SomeArguments {
	int N, M, K;
};

DWORD WINAPI myThreadProc(void* lpParameter) {
	//while (1)
	struct SomeArguments* s = (struct SomeArguments*)lpParameter;
	for(int i = 0; i < (s->N + s->M); ++i)
		printf("a");
}

/* how to do assembly in visual studio, but 32 bit only!
int a
__asm {
mov eax, a
}
*/

DWORD FindProcessId(const wchar_t *processname)
{
	PROCESSENTRY32 pe32;
	DWORD result = NULL;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Take a snapshot of all processes in the system.
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hProcessSnap) 
		return 0;

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);          // Clean the snapshot object
		return 0;
	}

	do
	{
		// this is bad because it only compared the first 'n' and stopped
		// no 0 termination
		//if (0 == _stricmp(processname, pe32.szExeFile))
		if (0 == _wcsicmp(processname, pe32.szExeFile))
		{
			result = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return result;
}


int main(int argc, char *argv[]) {

	//while (1)
	//	printf("c");

	int *ptr = NULL;


	/*DWORD tid;
	struct SomeArguments s;
	HANDLE hThread = CreateThread(NULL, 0, myThreadProc, &s, 0, &tid);

	while (1)
		printf("b");*/

	DWORD notepadPid = FindProcessId(L"notepad.exe");
	if (notepadPid == 0) {
		printf("failed to find process\n");
		return -1;
	}

	
	// open a handle to the process object that has this process-id
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, notepadPid);
	if (hProc == NULL) {
		printf("failed OpenProcess\n");
		return -1;
	}
	//"C:\\shy\\remote_dll.dll"

	
	// allocate some memory inside the notepad process
	// this will be like notepad called malloc()
	LPVOID remotePtr = VirtualAllocEx(hProc, NULL, 100, MEM_COMMIT, PAGE_READWRITE);
	if (remotePtr == NULL) {
		printf("failed VirtualAllocEx\n");
		return -1;
	}

	// copy this string from the runner process into the memory we allocated inside
	// notepad process
	//char *buffer = "C:\\shy\\remote_dll.dll";
	char* buffer = "C:\\Users\\Ofek\\Desktop\\coding with c\\Shy lessons\\lessons\\hello_worlds\\dll_inject\\x64\\Debug\\new_remote_dll.dll";
	SIZE_T written = 0;
	BOOL ret = WriteProcessMemory(hProc, remotePtr, buffer, strlen(buffer) + 1, &written);
	if (ret == 0) {
		printf("WriteProcessMemory failed");
		return -1;
	}

	DWORD tid;
	HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, LoadLibraryA, remotePtr, 0, &tid);

	

	return 0;
}
