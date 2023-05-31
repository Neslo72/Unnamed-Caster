#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>


int getProcId(const wchar_t* target)
{
	DWORD procID = 0;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	do {
		if (wcscmp(pe32.szExeFile, target) == 0)
		{
			CloseHandle(hSnapshot);
			procID = pe32.th32ProcessID;
			return procID;
		}
	} while (Process32Next(hSnapshot, &pe32));
	CloseHandle(hSnapshot);
	return procID;
}


// Print Error msg from GetLastError()
void printErr(const char* err_msg)
{
	DWORD err_id = GetLastError();
	LPSTR msg_buf = nullptr;
	DWORD size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, err_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg_buf, 0, NULL);
	printf("%s: %s\n", err_msg, msg_buf);
	LocalFree(msg_buf);
}


int main(int argc, char* argv[])
{
	const wchar_t* process = L"UNIst.exe";
	int procID = getProcId(process);
	if (!procID)
	{
		printf("Process UNIst.exe not found\n");
		return 1;
	}
	printf("PROCID : %d\n", procID);

	char dllPath[MAX_PATH];
	strcpy_s(dllPath, "D:\\Projects\\Unnamed-Caster\\Debug\\UNI_DLL.dll");  // Hardcoded for now
	printf("DLL Path name : %s\n", dllPath);

	HANDLE proc;
	LPVOID proc_addy;
	DWORD size = strlen(dllPath) + 1;

	proc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, false, procID);
	if (proc == NULL) 
	{
		printErr("OpenProcess");
		return 1;
	}

	proc_addy = VirtualAllocEx(proc, nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (proc_addy == NULL) 
	{
		printErr("VirtualAllocEx");
		return 1;
	}

	WriteProcessMemory(proc, proc_addy, dllPath, size, nullptr);
	if (CreateRemoteThread(proc, nullptr, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, proc_addy, 0, nullptr) == NULL) 
	{
		printErr("CreateRemoteThread");
		return 1;
	}
	CloseHandle(proc);

	printf("Injected\n");

	return 0;
}