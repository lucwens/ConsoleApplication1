#include <windows.h>
#include <iostream>
#include <string>

int main()
{
    // --- PART 1: Launch the console program hidden ---

    // Specify the path to your console program
    // Replace "YourConsoleProgram.exe" with the actual path to your program.
    // For demonstration, let's assume a simple program like "cmd.exe" for now.
    // If it's your own program, make sure it's compiled and accessible.
    std::string programPath = "C:\\Windows\\System32\\cmd.exe"; // Example: Command Prompt

    STARTUPINFO         StartUpInfo;
    PROCESS_INFORMATION ProcessInformation;

    ZeroMemory(&StartUpInfo, sizeof(StartUpInfo));
    StartUpInfo.cb = sizeof(StartUpInfo);
    ZeroMemory(&ProcessInformation, sizeof(ProcessInformation));
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

    std::cout << "Console program launched hidden. PID: " << ProcessInformation.dwProcessId << std::endl;
    std::cout << "Waiting for 5 seconds before attempting to show it..." << std::endl;

    // Wait for a few seconds (optional, for demonstration)
    Sleep(5000); // 5000 milliseconds = 5 seconds

    // --- PART 2: Show the console program ---

    HWND      hwndConsole = NULL;
    // Keep trying to find the window until it's found or a timeout occurs.
    // This is important because the console window might not be immediately available
    // after CreateProcess returns.
    int       attempts    = 0;
    const int maxAttempts = 20; // Try for up to 2 seconds (100ms * 20)
    while (hwndConsole == NULL && attempts < maxAttempts)
    {
        // Find the console window. Console windows typically have the class name "ConsoleWindowClass".
        // If you know the exact title of your console, you can use that as the second argument.
        // For cmd.exe, the default title is "Command Prompt".
        hwndConsole = FindWindow("ConsoleWindowClass", NULL); // Look for any console window
        if (hwndConsole == NULL)
        {
            hwndConsole = FindWindow(NULL, "Command Prompt"); // Try with default cmd title
        }
        if (hwndConsole == NULL)
        {
            // Wait a bit before retrying
            Sleep(100);
            attempts++;
        }
    }

    if (hwndConsole)
    {
        DWORD processID = GetWindowThreadProcessId(hwndConsole, NULL);
        if (processID == ProcessInformation.dwThreadId)
            std::cout << "Process correctly identified" << std::endl;
    }
    if (hwndConsole)
    {
        // Show the window
        ShowWindow(hwndConsole, SW_SHOW);
        std::cout << "Console program should now be visible." << std::endl;
        Sleep(5000); // 5000 milliseconds = 5 seconds
        ShowWindow(hwndConsole, SW_HIDE);
        std::cout << "Console program should now be visible." << std::endl;
    }
    else
    {
        std::cerr << "Failed to find the console window." << std::endl;
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
