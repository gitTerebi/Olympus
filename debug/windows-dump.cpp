#include "windows-dump.h"

#ifdef _WIN32

#include <windows.h>
#include <dbghelp.h>
#include <psapi.h>
#include <tlhelp32.h>

#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>

namespace {
struct ModuleInfo {
    DWORD64 base = 0;
    DWORD64 symbolBase = 0;
    DWORD size = 0;
    std::string name;
    std::string path;
};

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

std::string quote(const std::string& value) {
    return "\"" + value + "\"";
}

std::string addr2linePath() {
    std::string path = EZEUS_SOURCE_DIR;
    path += "\\build-deps\\llvm-mingw\\bin\\addr2line.exe";
    return path;
}

std::string currentExePath() {
    char path[MAX_PATH];
    const DWORD size = GetModuleFileNameA(nullptr, path, sizeof(path));
    if(!size || size >= sizeof(path)) return "";
    return path;
}

bool getMainModule(ModuleInfo& info) {
    const HMODULE module = GetModuleHandleA(nullptr);
    MODULEINFO moduleInfo;
    if(!GetModuleInformation(GetCurrentProcess(), module, &moduleInfo,
                             sizeof(moduleInfo))) {
        return false;
    }
    info.base = reinterpret_cast<DWORD64>(moduleInfo.lpBaseOfDll);
    const auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(info.base);
    const auto nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(
                info.base + dos->e_lfanew);
    info.symbolBase = nt->OptionalHeader.ImageBase;
    info.size = moduleInfo.SizeOfImage;
    info.path = currentExePath();
    info.name = info.path;
    const auto slash = info.name.find_last_of("\\/");
    if(slash != std::string::npos) info.name = info.name.substr(slash + 1);
    return true;
}

std::string readCommand(const std::string& command) {
    FILE* const pipe = _popen(command.c_str(), "r");
    if(!pipe) return "";

    std::string result;
    char buffer[512];
    while(std::fgets(buffer, sizeof(buffer), pipe)) {
        result += buffer;
    }
    _pclose(pipe);
    return result;
}

std::string symbolizeAddress(const DWORD64 address, const ModuleInfo& module) {
    if(address < module.base || address >= module.base + module.size) return "";
    if(module.path.empty()) return "";

    const DWORD64 symbolAddress = module.symbolBase + address - module.base;
    std::ostringstream command;
    command << quote(addr2linePath())
            << " -f -C -e " << quote(module.path)
            << " 0x" << std::hex << symbolAddress;

    auto result = readCommand(command.str());
    while(!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
        result.pop_back();
    }
    if(result.empty() || result == "??\n??:0" || result == "??\r\n??:0") {
        return "";
    }
    for(auto& c : result) {
        if(c == '\r' || c == '\n') c = ' ';
    }
    return result;
}

void writeException(FILE* const file, EXCEPTION_POINTERS* const ep) {
    const auto er = ep->ExceptionRecord;
    std::fprintf(file, "exception.code=0x%08lx\n", er->ExceptionCode);
    std::fprintf(file, "exception.flags=0x%08lx\n", er->ExceptionFlags);
    std::fprintf(file, "exception.address=%p\n", er->ExceptionAddress);
    std::fprintf(file, "thread.id=%lu\n", GetCurrentThreadId());
    std::fprintf(file, "process.id=%lu\n", GetCurrentProcessId());
}

void writeRegisters(FILE* const file, EXCEPTION_POINTERS* const ep) {
    const auto ctx = ep->ContextRecord;
#if defined(_M_X64) || defined(__x86_64__)
    std::fprintf(file, "\n[registers]\n");
    std::fprintf(file, "rip=0x%llx rsp=0x%llx rbp=0x%llx\n",
                 ctx->Rip, ctx->Rsp, ctx->Rbp);
    std::fprintf(file, "rax=0x%llx rbx=0x%llx rcx=0x%llx rdx=0x%llx\n",
                 ctx->Rax, ctx->Rbx, ctx->Rcx, ctx->Rdx);
    std::fprintf(file, "rsi=0x%llx rdi=0x%llx r8=0x%llx r9=0x%llx\n",
                 ctx->Rsi, ctx->Rdi, ctx->R8, ctx->R9);
    std::fprintf(file, "r10=0x%llx r11=0x%llx r12=0x%llx r13=0x%llx\n",
                 ctx->R10, ctx->R11, ctx->R12, ctx->R13);
    std::fprintf(file, "r14=0x%llx r15=0x%llx\n", ctx->R14, ctx->R15);
#elif defined(_M_IX86) || defined(__i386__)
    std::fprintf(file, "\n[registers]\n");
    std::fprintf(file, "eip=0x%lx esp=0x%lx ebp=0x%lx\n",
                 ctx->Eip, ctx->Esp, ctx->Ebp);
    std::fprintf(file, "eax=0x%lx ebx=0x%lx ecx=0x%lx edx=0x%lx\n",
                 ctx->Eax, ctx->Ebx, ctx->Ecx, ctx->Edx);
    std::fprintf(file, "esi=0x%lx edi=0x%lx\n", ctx->Esi, ctx->Edi);
#endif
}

void writeModules(FILE* const file) {
    std::fprintf(file, "\n[modules]\n");
    const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE |
                                                     TH32CS_SNAPMODULE32,
                                                     GetCurrentProcessId());
    if(snapshot == INVALID_HANDLE_VALUE) return;

    MODULEENTRY32 module;
    std::memset(&module, 0, sizeof(module));
    module.dwSize = sizeof(module);
    if(Module32First(snapshot, &module)) {
        do {
            std::fprintf(file, "%p %lu %s %s\n",
                         module.modBaseAddr,
                         static_cast<unsigned long>(module.modBaseSize),
                         module.szModule,
                         module.szExePath);
        } while(Module32Next(snapshot, &module));
    }
    CloseHandle(snapshot);
}

void writeStack(FILE* const file, EXCEPTION_POINTERS* const ep) {
    std::fprintf(file, "\n[stack]\n");
    ModuleInfo mainModule;
    const bool haveMainModule = getMainModule(mainModule);

    const HANDLE process = GetCurrentProcess();
    const HANDLE thread = GetCurrentThread();
    SymInitialize(process, nullptr, TRUE);

    STACKFRAME64 frame;
    std::memset(&frame, 0, sizeof(frame));
    auto ctx = *ep->ContextRecord;

#if defined(_M_X64) || defined(__x86_64__)
    DWORD machine = IMAGE_FILE_MACHINE_AMD64;
    frame.AddrPC.Offset = ctx.Rip;
    frame.AddrFrame.Offset = ctx.Rbp;
    frame.AddrStack.Offset = ctx.Rsp;
#elif defined(_M_IX86) || defined(__i386__)
    DWORD machine = IMAGE_FILE_MACHINE_I386;
    frame.AddrPC.Offset = ctx.Eip;
    frame.AddrFrame.Offset = ctx.Ebp;
    frame.AddrStack.Offset = ctx.Esp;
#else
    DWORD machine = 0;
#endif
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Mode = AddrModeFlat;

    char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
    auto symbol = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
    std::memset(symbol, 0, sizeof(symbolBuffer));
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;

    for(int i = 0; machine && i < 64; i++) {
        if(!StackWalk64(machine, process, thread, &frame, &ctx, nullptr,
                        SymFunctionTableAccess64, SymGetModuleBase64,
                        nullptr)) {
            break;
        }
        if(!frame.AddrPC.Offset) break;

        DWORD64 displacement = 0;
        if(SymFromAddr(process, frame.AddrPC.Offset, &displacement, symbol)) {
            std::fprintf(file, "#%02d 0x%llx %s+0x%llx\n",
                         i,
                         static_cast<unsigned long long>(frame.AddrPC.Offset),
                         symbol->Name,
                         static_cast<unsigned long long>(displacement));
        } else {
            std::fprintf(file, "#%02d 0x%llx\n",
                         i,
                         static_cast<unsigned long long>(frame.AddrPC.Offset));
        }
        if(haveMainModule) {
            const auto resolved = symbolizeAddress(frame.AddrPC.Offset,
                                                   mainModule);
            if(!resolved.empty()) {
                std::fprintf(file, "    %s\n", resolved.c_str());
            }
        }
    }
    SymCleanup(process);
}

void writeTextDump(const std::string& path, EXCEPTION_POINTERS* const ep) {
    FILE* const file = std::fopen(path.c_str(), "w");
    if(!file) return;

    writeException(file, ep);
    writeRegisters(file, ep);
    writeStack(file, ep);
    writeModules(file);
    std::fclose(file);
}

LONG WINAPI writeDump(EXCEPTION_POINTERS* exceptionPointers) {
    const auto basePath = dumpBasePath();
    const auto path = basePath + ".dmp";
    writeTextDump(basePath + ".txt", exceptionPointers);

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
