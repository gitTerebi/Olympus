#include "windows-dump.h"

#ifdef _WIN32

#include <windows.h>
#include <dbghelp.h>

#include <cstdio>
#include <string>

namespace {
std::string dumpBasePath() {
    SYSTEMTIME st;
    GetLocalTime(&st);

    std::string dir = EZEUS_SOURCE_DIR;
    dir += "\\debug";
    CreateDirectoryA(dir.c_str(), nullptr);

    char name[MAX_PATH];
    std::snprintf(name, sizeof(name),
                  "%s\\ezeus-%04u%02u%02u-%02u%02u%02u",
                  dir.c_str(),
                  st.wYear, st.wMonth, st.wDay,
                  st.wHour, st.wMinute, st.wSecond);
    return name;
}

LONG WINAPI writeDump(EXCEPTION_POINTERS* exceptionPointers) {
    const auto basePath = dumpBasePath();
    const auto path = basePath + ".dmp";

    const HANDLE file = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr,
                                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                                    nullptr);
    if(file == INVALID_HANDLE_VALUE) return EXCEPTION_EXECUTE_HANDLER;

    MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
    exceptionInfo.ThreadId = GetCurrentThreadId();
    exceptionInfo.ExceptionPointers = exceptionPointers;
    exceptionInfo.ClientPointers = FALSE;

    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file,
                      MiniDumpWithIndirectlyReferencedMemory,
                      &exceptionInfo, nullptr, nullptr);
    CloseHandle(file);
    return EXCEPTION_EXECUTE_HANDLER;
}
}

void installWindowsDumpHandler() {
    SetUnhandledExceptionFilter(writeDump);
}

#else

void installWindowsDumpHandler() {}

#endif
