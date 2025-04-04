#pragma once

using PID = DWORD;
using PROCESS = HANDLE;

struct SelectedProcess {
    PID pid{ NULL };
    PROCESS process {nullptr};
};

namespace Process {

    void EnableDebugPriv();

    inline std::map<std::wstring, PID> currentsProcess;

    inline SelectedProcess selectedProcess{ NULL };

    void GetAllProcess();
}

//void refindBytePatternInProcessMemory(PROCESS process, void* pattern, size_t patternLen, std::vector<void*>& found)
//{
//	if (patternLen == 0) { return; }
//
//	auto newFound = findBytePatternInProcessMemory(process, pattern, patternLen);
//
//	std::vector<void*> intersect;
//	intersect.resize(std::min(found.size(), newFound.size()));
//
//	std::set_intersection(found.begin(), found.end(),
//		newFound.begin(), newFound.end(),
//		intersect.begin());
//
//	intersect.erase(std::remove(intersect.begin(), intersect.end(), nullptr), intersect.end());
//
//	found = std::move(intersect);
//}
//
////http://kylehalladay.com/blog/2020/05/20/Rendering-With-Notepad.html
//std::vector<void*> findBytePatternInProcessMemory(PROCESS process, void* pattern, size_t patternLen)
//{
//	if (patternLen == 0) { return {}; }
//
//	std::vector<void*> returnVec;
//	returnVec.reserve(1000);
//
//	auto query = initVirtualQuery(process);
//
//	if (!query.oppened())
//		return {};
//
//	void* low = nullptr;
//	void* hi = nullptr;
//	int flags = memQueryFlags_None;
//
//	while (getNextQuery(query, low, hi, flags))
//	{
//		if ((flags | memQueryFlags_Read) && (flags | memQueryFlags_Write))
//		{
//			//search for our byte patern
//			size_t size = (char*)hi - (char*)low;
//			char* localCopyContents = new char[size];
//			if (
//				readMemory(process, low, size, localCopyContents)
//				)
//			{
//				char* cur = localCopyContents;
//				size_t curPos = 0;
//				while (curPos < size - patternLen + 1)
//				{
//					if (memcmp(cur, pattern, patternLen) == 0)
//					{
//						returnVec.push_back((char*)low + curPos);
//					}
//					curPos++;
//					cur++;
//				}
//			}
//			delete[] localCopyContents;
//		}
//	}
//
//	return returnVec;
//}