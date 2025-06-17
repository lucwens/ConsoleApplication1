#include <windows.h>
#include <iostream>
#include <string>
#include <conio.h> // For _kbhit and _getch
#include <thread>  // For std::this_thread::sleep_for
#include <chrono>  // For std::chrono::milliseconds
#include <vector>  // For dynamic string for CreateProcess

HWND GetConsoleWindowFromProcessId(DWORD pid)
{
    struct HandleData
    {
        DWORD pid;
        HWND  hwnd;
    };

    HandleData data{pid, NULL};

    auto enumWindowsCallback = [](HWND hWnd, LPARAM lParam) -> BOOL
    {
        HandleData *pData = reinterpret_cast<HandleData *>(lParam);
        DWORD       windowPid;
        GetWindowThreadProcessId(hWnd, &windowPid);

        if (windowPid == pData->pid)
        {
            TCHAR className[64];
            GetClassName(hWnd, className, sizeof(className) / sizeof(TCHAR));
            if (strcmp(className, "ConsoleWindowClass") == 0)
            {
                pData->hwnd = hWnd;
                return FALSE; // Stop enumerating
            }
        }
        return TRUE;
    };

    EnumWindows(enumWindowsCallback, reinterpret_cast<LPARAM>(&data));
    return data.hwnd;
}

void ShowHideWindow(HWND hConsoleWnd, bool show)
{
    if (hConsoleWnd == NULL)
        return;

    if (show)
    {
        ShowWindow(hConsoleWnd, SW_SHOW);
        SetForegroundWindow(hConsoleWnd);
        SetWindowPos(hConsoleWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

        long style = GetWindowLong(hConsoleWnd, GWL_EXSTYLE);
        if ((style & WS_EX_TOOLWINDOW) != 0)
        {
            SetWindowLong(hConsoleWnd, GWL_EXSTYLE, style & ~WS_EX_TOOLWINDOW);
        }
    }
    else
    {
        ShowWindow(hConsoleWnd, SW_HIDE);

        long style = GetWindowLong(hConsoleWnd, GWL_EXSTYLE);
        if ((style & WS_EX_TOOLWINDOW) == 0)
        {
            SetWindowLong(hConsoleWnd, GWL_EXSTYLE, style | WS_EX_TOOLWINDOW);
        }
    }
}

//---

int main()
{
    // --- PART 1: Launch the console program hidden ---

    std::string       programPath = "C:\\Windows\\System32\\cmd.exe"; // Example: Command Prompt
    // It's better to use a mutable buffer for CreateProcess
    std::vector<char> cmdLine(programPath.begin(), programPath.end());
    cmdLine.push_back('\0'); // Null-terminate

    STARTUPINFO         StartUpInfo{};
    PROCESS_INFORMATION ProcessInformation{};
    StartUpInfo.cb          = sizeof(StartUpInfo);

    // Set wShowWindow to SW_HIDE to make the window initially hidden
    StartUpInfo.dwFlags     = STARTF_USESHOWWINDOW;
    StartUpInfo.wShowWindow = SW_SHOW; // Use SW_SHOW to create the window hidden, but it will be hidden later

    // Create the process
    BOOL success            = CreateProcess(NULL,               // No module name (use command line)
                                            cmdLine.data(),     // Command line (mutable)
                                            NULL,               // Process handle not inheritable
                                            NULL,               // Thread handle not inheritable
                                            FALSE,              // Set handle inheritance to FALSE
                                            CREATE_NEW_CONSOLE, // Create a new console for the process
                                            NULL,               // Use parent's environment block
                                            NULL,               // Use parent's starting directory
                                            &StartUpInfo,       // Pointer to STARTUPINFO structure
                                            &ProcessInformation // Pointer to PROCESS_INFORMATION structure
               );

    if (!success)
    {
        std::cerr << "Failed to create process. Error: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "Console program launched hidden. PID: " << ProcessInformation.dwProcessId << std::endl;
    std::cout << "Press 's' to show, 'h' to hide, 'q' to quit." << std::endl;

    // Optional: Wait for the child process to be idle.
    // This helps ensure the console window is fully set up before we try to control it.
    WaitForInputIdle(ProcessInformation.hProcess, 5000); // Wait up to 5 seconds
    HWND childConsoleWnd = nullptr;

    // Try to get and cache the window handle
    for (int i = 0; i < 50; ++i)
    {
        childConsoleWnd = GetConsoleWindowFromProcessId(ProcessInformation.dwProcessId);
        if (childConsoleWnd != nullptr)
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (childConsoleWnd == nullptr)
    {
        std::cerr << "Failed to find child console window!" << std::endl;
        TerminateProcess(ProcessInformation.hProcess, 0);
        return 1;
    }

    bool continueExecution = true;
    while (continueExecution)
    {
        if (_kbhit())
        {
            char c = _getch();
            switch (c)
            {
                case 'q':
                    continueExecution = false;
                    break; // Quit the program
                case 's':
                    // Show the console window
                    ShowHideWindow(childConsoleWnd, true);
                    std::cout << "Console program should now be visible." << std::endl;
                    break;
                case 'h':
                    ShowHideWindow(childConsoleWnd, false);
                    std::cout << "Console program should now be hidden." << std::endl;
                    break;
                default:
                    // Optionally, print a message for unknown keys
                    // std::cout << "Unknown command: " << c << std::endl;
                    break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep to avoid busy waiting
    }

    // Terminate the launched process when done
    TerminateProcess(ProcessInformation.hProcess, 0);

    // Close process and thread handles. This is important to avoid resource leaks.
    CloseHandle(ProcessInformation.hProcess);
    CloseHandle(ProcessInformation.hThread);

    std::cout << "Exiting." << std::endl;

    return 0;
}