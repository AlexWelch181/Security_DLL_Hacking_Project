
#include <iostream>
#include <windows.h>
#include <string>
#include <thread>let 
#include <libloaderapi.h>
#include <psapi.h>
#include <VersionHelpers.h>

using namespace std;

void get_proc_id(const char* window_title, DWORD& process_id)
{
    GetWindowThreadProcessId(FindWindow(0, window_title), &process_id);
}

void error(const char* error_title, const char* error_message)
{
    std::cerr << "Error: " << error_title << " - " << error_message << std::endl;
    exit(-1);
}

bool file_exists(string file_name)
{
    struct stat buffer;
    return (stat(file_name.c_str(), &buffer) == 0);
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD dwThreadId, dwProcessId;
    HINSTANCE hInstance;
    char String[255];
    if (!hWnd)
        return TRUE;        // Not a window
    if (!::IsWindowVisible(hWnd))
        return TRUE;        // Not visible
    if (!SendMessage(hWnd, WM_GETTEXT, sizeof(String), (LPARAM)String))
        return TRUE;        // No window title
    hInstance = (HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE);
    dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
    std::cout << "PID: " << dwProcessId << '\t' << String << '\t' << std::endl;
    return TRUE;
}

BOOL InjectDLL(DWORD process_id, char* dll_path)
{
    HANDLE h_process = OpenProcess(PROCESS_ALL_ACCESS, NULL, process_id);
    if (!h_process)
    {
        error("OpenProcess", "Failed to open a handle to the process");
    }

    void* allocated_memory = VirtualAllocEx(h_process, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!allocated_memory)
    {
        error("VirtualAllocEX", "Failed to allocate memory");
    }

    if (!WriteProcessMemory(h_process, allocated_memory, dll_path, MAX_PATH, nullptr))
    {
        error("WriteProcessMemory", "Failed to write memory into the process");
    }

    HANDLE h_thread = CreateRemoteThread(h_process, nullptr, NULL, LPTHREAD_START_ROUTINE(LoadLibraryA), allocated_memory, NULL, nullptr);
    if (!h_thread)
    {
        error("CreatedRemoteThread", "Failed to create remote thread");
    }

    CloseHandle(h_process);
    VirtualFreeEx(h_process, allocated_memory, NULL, MEM_RELEASE);

    std::cout << "Injection successful";
    return true;
}

int main()
{
    DWORD process_id = NULL;
    char dll_path[MAX_PATH];
    const char* dll_name = "PwnAdventureHax.dll";

    if (IsWindowsXPOrGreater()) {
        std::cout << "Available Targets:\n\n" << std::endl;
        EnumWindows(EnumWindowsProc, NULL);
        std::cout << "\nPick Target ProcessID" << std::endl;
        DWORD PID;
        std::cin >> PID;

        std::cout << "\nGive dll path" << std::endl;
        std::cin >> dll_path;

        if (!file_exists(dll_path))
        {
            error("File not found", "The specified DLL file does not exist.");
        }

        InjectDLL(PID, dll_path);
    }
    else {
        std::cout << "Method not supported by OS. Terminating" << std::endl;
        return 0;
    }

    return 0;
}
