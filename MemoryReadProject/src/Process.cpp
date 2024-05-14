#include "../Include/global.h"


void Process::EnableDebugPriv()
{
	HANDLE hToken;
	LUID luid;
	TOKEN_PRIVILEGES tkp;

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luid;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL);

	CloseHandle(hToken);
}

void Process::GetAllProcess() {
    Process::EnableDebugPriv();

    if (Process::currentsProcess.size()) {
        Process::currentsProcess.clear();
    }

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &entry) == TRUE) {
        do
        {
            Process::currentsProcess[entry.szExeFile] = entry.th32ProcessID;
            //if (wcscmp(entry.szExeFile, /*processName*/) == 0) {
                //printf("process name: %ls / PID: %d\n", &entry.szExeFile, entry.th32ProcessID);
            //}
        } while (Process32Next(snapshot, &entry) == TRUE);
    }
}