#include "windows-dump.h"

#ifdef _WIN32

#include <windows.h>
#include <dbghelp.h>

#include <cstdio>
#include <cctype>
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

bool shouldSuppress(EXCEPTION_POINTERS* ep) {
    if(!ep || !ep->ExceptionRecord) return false;
    const DWORD code = ep->ExceptionRecord->ExceptionCode;
    if(code == 0x40010006 /*DBG_PRINTEXCEPTION_C*/ ||
       code == 0x4001000A /*DBG_PRINTEXCEPTION_WIDE_C*/ ||
       code == 0x406D1388 /*MS_VC_EXCEPTION thread name*/) return true;
    if((code & 0xF0000000) != 0xC0000000) return true;

    const void* addr = ep->ExceptionRecord->ExceptionAddress;
    if(!addr) return false;
    HMODULE mod = nullptr;
    if(GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          static_cast<LPCSTR>(addr), &mod) && mod) {
        char name[MAX_PATH] = {0};
        if(GetModuleFileNameA(mod, name, sizeof(name))) {
            std::string s = name;
            for(auto& c : s) c = static_cast<char>(tolower(c));
            if(s.find("sdl2.dll") != std::string::npos ||
               s.find("ntdll.dll") != std::string::npos ||
               s.find("kernel32.dll") != std::string::npos ||
               s.find("kernelbase.dll") != std::string::npos) return true;
        }
    }
    return false;
}

LONG WINAPI writeDump(EXCEPTION_POINTERS* exceptionPointers) {
    if(shouldSuppress(exceptionPointers)) return EXCEPTION_CONTINUE_SEARCH;

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
