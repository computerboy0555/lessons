
#include <Windows.h> // for win32 functions
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



int main(int argc, char *argv[]) {

	//while (1)
	//	printf("c");

	int *ptr = NULL;


	/*DWORD tid;
	struct SomeArguments s;
	HANDLE hThread = CreateThread(NULL, 0, myThreadProc, &s, 0, &tid);

	while (1)
		printf("b");*/
	
	// open a handle to the process object that has this process-id
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 9680);
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
	char* buffer = "C:\\shy\\lessons\\hello_worlds\\dll_inject\\x64\\Debug\\new_remote_dll.dll";
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
