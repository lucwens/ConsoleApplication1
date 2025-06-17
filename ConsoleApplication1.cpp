#include <windows.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <thread>

HWND GetWindowHandle(PROCESS_INFORMATION &processInfo)
{
    HWND  hWnd      = NULL;
    DWORD processID = processInfo.dwProcessId;
    // Enumerate all top-level windows to find the one that belongs to the process
    EnumWindows(
        [](HWND hwnd, LPARAM lParam) -> BOOL
        {
            DWORD windowProcessID;
            GetWindowThreadProcessId(hwnd, &windowProcessID);
            if (windowProcessID == *(DWORD *)lParam)
            {
                *(HWND *)lParam = hwnd; // Store the found window handle
                return FALSE;           // Stop enumeration
            }
            return TRUE; // Continue enumeration
        },
        (LPARAM)&hWnd);
    return hWnd;
}

bool IsConsoleVisible(PROCESS_INFORMATION &processInfo)
{
    HWND hConsoleWnd = GetWindowHandle(processInfo);
    return (hConsoleWnd != NULL) && IsWindowVisible(hConsoleWnd);
}

void ShowHideWindow(PROCESS_INFORMATION &processInfo, bool show)
{
    HWND hConsoleWnd = GetWindowHandle(processInfo);

    if (hConsoleWnd == NULL)
        return;

    if (show)
    {
        ShowWindow(hConsoleWnd, SW_SHOW);
        SetForegroundWindow(hConsoleWnd);

        // Make sure it is shown properly
        SetWindowLong(hConsoleWnd, GWL_EXSTYLE, GetWindowLong(hConsoleWnd, GWL_EXSTYLE) & ~WS_EX_TOOLWINDOW); // Show in taskbar
    }
    else
    {
        ShowWindow(hConsoleWnd, SW_HIDE);

        // Hide from taskbar by setting the extended window style to WS_EX_TOOLWINDOW
        SetWindowLong(hConsoleWnd, GWL_EXSTYLE, GetWindowLong(hConsoleWnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW); // Hide from taskbar
    }
}

int main()
{
    // --- PART 1: Launch the console program hidden ---

    // Specify the path to your console program
    // Replace "YourConsoleProgram.exe" with the actual path to your program.
    // For demonstration, let's assume a simple program like "cmd.exe" for now.
    // If it's your own program, make sure it's compiled and accessible.
    std::string programPath = "C:\\Windows\\System32\\cmd.exe"; // Example: Command Prompt replace with engine afterwards

    STARTUPINFO         StartUpInfo{};
    PROCESS_INFORMATION ProcessInformation{};
    StartUpInfo.cb          = sizeof(StartUpInfo);

    // Set wShowWindow to SW_HIDE to make the window initially hidden
    StartUpInfo.dwFlags     = STARTF_USESHOWWINDOW;
    StartUpInfo.wShowWindow = SW_HIDE; // Hide the window

    // Create the process
    BOOL success            = CreateProcess(NULL,                       // No module name (use command line)
                                            (LPSTR)programPath.c_str(), // Command line
                                            NULL,                       // Process handle not inheritable
                                            NULL,                       // Thread handle not inheritable
                                            FALSE,                      // Set handle inheritance to FALSE
                                            CREATE_NEW_CONSOLE,         // Create a new console for the process
                                            NULL,                       // Use parent's environment block
                                            NULL,                       // Use parent's starting directory
                                            &StartUpInfo,               // Pointer to STARTUPINFO structure
                                            &ProcessInformation         // Pointer to PROCESS_INFORMATION structure
               );

    if (!success)
    {
        std::cerr << "Failed to create process. Error: " << GetLastError() << std::endl;
        return 1;
    }

    bool continueExecution = true;
    while (continueExecution)
    {
        if (_kbhit())
        {
            char c = getchar();
            switch (c)
            {
                case 'q':
                    continueExecution = false;
                    break; // Quit the program
                case 's':
                    // Show the console window
                    ShowHideWindow(ProcessInformation, true);
                    std::cout << "Console program should now be visible." << std::endl;
                    break;
                case 'h':
                    ShowHideWindow(ProcessInformation, false);
                    std::cout << "Console program should now be hidden." << std::endl;
                    break;
                default:
                    break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep to avoid busy waiting
    }

    // Wait for the launched process to finish (optional)
    // WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles. This is important to avoid resource leaks.
    CloseHandle(ProcessInformation.hProcess);
    CloseHandle(ProcessInformation.hThread);

    std::cout << "Press any key to exit." << std::endl;
    std::cin.get();

    return 0;
}
