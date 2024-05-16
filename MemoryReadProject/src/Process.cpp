#include "../Include/global.h"


void SelectedProcess::openProcess() {
	this->process = OpenProcess(
		PROCESS_VM_READ | PROCESS_QUERY_INFORMATION |
		PROCESS_VM_WRITE | PROCESS_VM_OPERATION
		, 0, this->pid);

	if ((this->process == INVALID_HANDLE_VALUE) || (this->process == 0)) {
		this->process = 0;
		this->pid = 0;
	}
}

template <typename T>
void SelectedProcess::ReadMemory(void* start, size_t size, T const& buff) {
	if (ReadProcessMemory(this->process, start, buff, size, nullptr)) {
		return buff;
	}
	return buff;
}

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

unsigned long show_module(MEMORY_BASIC_INFORMATION info, int& count) {
	unsigned long usage = 0;

	if ((info.State == MEM_COMMIT) && (info.AllocationProtect == PAGE_READWRITE || info.AllocationProtect == PAGE_READONLY))
		usage += info.RegionSize;

	Process::currentMemoryModulesInfo[count] = info;

	return usage;
}

//GetMemoryPages
void Process::GetMemoryPages() {
	Process::currentMemoryModulesInfo.clear();

	unsigned long usage = 0;

	unsigned char* p = NULL;

	MEMORY_BASIC_INFORMATION info;

	int count{ 0 };
	for (p = NULL;
		VirtualQueryEx(Process::selectedProcess.process, p, &info, sizeof(info)) == sizeof(info);
		p += info.RegionSize)
	{
		usage += show_module(info, count);
		++count;
	}
}

void Process::DrawMemoryPages(const int& currentDrawPageIndex) {
	if (!Process::currentMemoryModulesInfo.size()) {
		return;
	}

	SYSTEM_INFO sSysInfo = { };
	GetSystemInfo(&sSysInfo);

	ImGui::Text("BaseAddress: %p RegionSize: (%zu) pages count: %d",
		Process::currentMemoryModulesInfo[currentDrawPageIndex].BaseAddress,
		Process::currentMemoryModulesInfo[currentDrawPageIndex].RegionSize,
		//Process::currentMemoryModulesInfo[currentDrawPageIndex].RegionSize / 1024,
		Process::currentMemoryModulesInfo[currentDrawPageIndex].RegionSize / sSysInfo.dwPageSize);

	if (ImGui::IsItemClicked()) {
		OpenClipboard(NULL);
		EmptyClipboard();

		char* str = (char*)malloc(20 * sizeof(char));
		snprintf(str, 20, "%p", Process::currentMemoryModulesInfo[currentDrawPageIndex].BaseAddress);

		HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, strlen(str) + 1);

		if (!hg) {
			CloseClipboard();
		}

		memcpy(GlobalLock(hg), str, strlen(str) + 1);
		GlobalUnlock(hg);
		SetClipboardData(CF_TEXT, hg);
		CloseClipboard();
		GlobalFree(hg);

	}

	ImGui::SameLine();

	switch (Process::currentMemoryModulesInfo[currentDrawPageIndex].State) {
	case MEM_COMMIT:
		ImGui::Text("Committed");
		break;
	case MEM_RESERVE:
		ImGui::Text("Reserved");
		break;
	case MEM_FREE:
		ImGui::Text("Free");
		break;
	}

	ImGui::SameLine();

	switch (Process::currentMemoryModulesInfo[currentDrawPageIndex].Type) {
	case MEM_IMAGE:
		ImGui::Text("Code Module");
		break;
	case MEM_MAPPED:
		ImGui::Text("Mapped     ");
		break;
	case MEM_PRIVATE:
		ImGui::Text("Private    ");
	}

	ImGui::SameLine();

	int guard = 0, nocache = 0;

	if (Process::currentMemoryModulesInfo[currentDrawPageIndex].AllocationProtect & PAGE_NOCACHE)
		nocache = 1;
	if (Process::currentMemoryModulesInfo[currentDrawPageIndex].AllocationProtect & PAGE_GUARD)
		guard = 1;

	Process::currentMemoryModulesInfo[currentDrawPageIndex].AllocationProtect &= ~(PAGE_GUARD | PAGE_NOCACHE);

	switch (Process::currentMemoryModulesInfo[currentDrawPageIndex].AllocationProtect) {
	case PAGE_READONLY:
		ImGui::Text("Read Only");
		break;
	case PAGE_READWRITE:
		ImGui::Text("Read/Write");
		break;
	case PAGE_WRITECOPY:
		ImGui::Text("Copy on Write");
		break;
	case PAGE_EXECUTE:
		ImGui::Text("Execute only");
		break;
	case PAGE_EXECUTE_READ:
		ImGui::Text("Execute/Read");
		break;
	case PAGE_EXECUTE_READWRITE:
		ImGui::Text("Execute/Read/Write");
		break;
	case PAGE_EXECUTE_WRITECOPY:
		ImGui::Text("COW Executable");
		break;
	}

	ImGui::SameLine();

	if (guard)
		ImGui::Text("guard page");
	if (nocache)
		ImGui::Text("non-cacheable");

	ImGui::SameLine();

	int data = 0;

	try
	{




		/*char bufferValue[256];
		std::sprintf(bufferValue, "%s");*/

		ImGui::Text("");
		ImGui::Text("\n");
	}
	catch (const std::exception&)
	{
		ImGui::Text("\n");
	}
}

//Get Modules
void Process::GetModules() {
	HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, Process::selectedProcess.pid);
	MODULEENTRY32 me32 = MODULEENTRY32();
	me32.dwSize = sizeof(MODULEENTRY32);

	if (hModuleSnap == INVALID_HANDLE_VALUE || hModuleSnap == 0) {
		CloseHandle(hModuleSnap);
		return;
	}

	int moduleCount{ 0 };

	if (Module32First(hModuleSnap, &me32) == TRUE) {
		Process::currentProcessModules[moduleCount] = me32;
		++moduleCount;
	}
	else
	{
		CloseHandle(hModuleSnap);
		return;
	}


	while (Module32Next(hModuleSnap, &me32) == TRUE) {
		Process::currentProcessModules[moduleCount] = me32;
		++moduleCount;
	}
}

void Process::DrawModules(const int& currentDrawModuleIndex) {
	ImGui::Text("ModuleName: %ls \\ ModBaseAddr: %p \\ ModBaseSize: %lu ",
		Process::currentProcessModules[currentDrawModuleIndex].szModule,
		Process::currentProcessModules[currentDrawModuleIndex].modBaseAddr,
		Process::currentProcessModules[currentDrawModuleIndex].modBaseSize
	);
}