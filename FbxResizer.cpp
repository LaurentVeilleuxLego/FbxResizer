// FbxResizer.cpp : Defines the entry point for the application.
//

#include "FbxResizer.h"
#include "FbxProcessor.h"

using namespace std;
namespace fs = std::filesystem;

// Global variables
HWND g_hEditSdkPath = NULL;
HWND g_hEditFbxFolder = NULL;
HWND g_hRadioX = NULL;
HWND g_hRadioY = NULL;
HWND g_hRadioZ = NULL;
HWND g_hEditResizeFactor = NULL;
HWND g_hEditLog = NULL;
HFONT g_hFont = NULL;

// Forward declarations
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void CreateControls(HWND hWnd);
void BrowseForFolder(HWND hWnd, HWND hEdit, const wchar_t* title);
void ProcessFBXFiles(HWND hWnd);
void LogMessage(const std::wstring& message);
std::wstring GetAxisName();
std::wstring FormatResizeFactor(double factor);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Initialize common controls
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icex);

	// Register window class
	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = L"FBXFileResizerClass";

	if (!RegisterClassExW(&wcex))
	{
		MessageBoxW(NULL, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Create main window
	HWND hWnd = CreateWindowExW(
		0,
		L"FBXFileResizerClass",
		L"FBX File Resizer",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		MessageBoxW(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Message loop
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		CreateControls(hWnd);
		break;

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDC_BTN_BROWSE_SDK:
			BrowseForFolder(hWnd, g_hEditSdkPath, L"Select FBX SDK DLL Folder");
			break;

		case IDC_BTN_BROWSE_FBX:
			BrowseForFolder(hWnd, g_hEditFbxFolder, L"Select FBX Folder");
			break;

		case IDC_BTN_PROCESS:
			ProcessFBXFiles(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_DESTROY:
		if (g_hFont)
			DeleteObject(g_hFont);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void CreateControls(HWND hWnd)
{
	// Create font
	g_hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

	int yPos = 15;

	// FBX SDK DLL Folder section
	CreateWindowW(L"STATIC", L"FBX SDK DLL Folder (e.g., C:\\...\\FBX SDK\\2020.3.4\\lib\\vs2021\\x64\\release)",
		WS_VISIBLE | WS_CHILD,
		15, yPos, 650, 20,
		hWnd, NULL, NULL, NULL);
	yPos += 25;

	g_hEditSdkPath = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT",
		L"C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.3.4\\lib\\vs2021\\x64\\release",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		15, yPos, 550, 25,
		hWnd, (HMENU)IDC_EDIT_SDK_PATH, NULL, NULL);
	SendMessage(g_hEditSdkPath, WM_SETFONT, (WPARAM)g_hFont, TRUE);

	CreateWindowW(L"BUTTON", L"Browse...",
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		575, yPos, 90, 25,
		hWnd, (HMENU)IDC_BTN_BROWSE_SDK, NULL, NULL);
	yPos += 40;

	// Select FBX Folder section
	CreateWindowW(L"STATIC", L"Select FBX Folder",
		WS_VISIBLE | WS_CHILD,
		15, yPos, 650, 20,
		hWnd, NULL, NULL, NULL);
	yPos += 25;

	g_hEditFbxFolder = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT",
		L"C:\\Git\\Atom2\\TestProjects\\Play_Atom\\Content\\VAD\\ModularSetup",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		15, yPos, 550, 25,
		hWnd, (HMENU)IDC_EDIT_FBX_FOLDER, NULL, NULL);
	SendMessage(g_hEditFbxFolder, WM_SETFONT, (WPARAM)g_hFont, TRUE);

	CreateWindowW(L"BUTTON", L"Browse...",
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		575, yPos, 90, 25,
		hWnd, (HMENU)IDC_BTN_BROWSE_FBX, NULL, NULL);
	yPos += 40;

	// Select Resize Axis section
	CreateWindowW(L"STATIC", L"Select Resize Axis",
		WS_VISIBLE | WS_CHILD,
		15, yPos, 650, 20,
		hWnd, NULL, NULL, NULL);
	yPos += 25;

	g_hRadioX = CreateWindowW(L"BUTTON", L"X Axis",
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		30, yPos, 80, 20,
		hWnd, (HMENU)IDC_RADIO_X, NULL, NULL);

	g_hRadioY = CreateWindowW(L"BUTTON", L"Y Axis",
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		130, yPos, 80, 20,
		hWnd, (HMENU)IDC_RADIO_Y, NULL, NULL);

	g_hRadioZ = CreateWindowW(L"BUTTON", L"Z Axis",
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		230, yPos, 80, 20,
		hWnd, (HMENU)IDC_RADIO_Z, NULL, NULL);

	// Set Y Axis as default
	SendMessage(g_hRadioY, BM_SETCHECK, BST_CHECKED, 0);
	yPos += 35;

	// Resize Amount section
	CreateWindowW(L"STATIC", L"Resize Amount (Multiplier)",
		WS_VISIBLE | WS_CHILD,
		15, yPos, 650, 20,
		hWnd, NULL, NULL, NULL);
	yPos += 25;

	CreateWindowW(L"STATIC", L"Resize Factor:",
		WS_VISIBLE | WS_CHILD,
		30, yPos + 3, 100, 20,
		hWnd, NULL, NULL, NULL);

	g_hEditResizeFactor = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"2.00",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
		140, yPos, 120, 25,
		hWnd, (HMENU)IDC_EDIT_RESIZE_FACTOR, NULL, NULL);
	SendMessage(g_hEditResizeFactor, WM_SETFONT, (WPARAM)g_hFont, TRUE);

	CreateWindowW(L"STATIC", L"Example: 2.0 = double size, 0.5 = half size",
		WS_VISIBLE | WS_CHILD,
		270, yPos + 3, 350, 20,
		hWnd, NULL, NULL, NULL);
	yPos += 40;

	// Process button
	CreateWindowW(L"BUTTON", L"Process FBX Files",
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		15, yPos, 650, 35,
		hWnd, (HMENU)IDC_BTN_PROCESS, NULL, NULL);
	yPos += 50;

	// Processing Log section
	CreateWindowW(L"STATIC", L"Processing Log:",
		WS_VISIBLE | WS_CHILD,
		15, yPos, 650, 20,
		hWnd, NULL, NULL, NULL);
	yPos += 25;

	g_hEditLog = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
		WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
		15, yPos, 650, 200,
		hWnd, (HMENU)IDC_EDIT_LOG, NULL, NULL);
	SendMessage(g_hEditLog, WM_SETFONT, (WPARAM)g_hFont, TRUE);
}

void BrowseForFolder(HWND hWnd, HWND hEdit, const wchar_t* title)
{
	BROWSEINFOW bi = {};
	bi.hwndOwner = hWnd;
	bi.lpszTitle = title;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
	if (pidl != NULL)
	{
		wchar_t path[MAX_PATH];
		if (SHGetPathFromIDListW(pidl, path))
		{
			SetWindowTextW(hEdit, path);
		}
		CoTaskMemFree(pidl);
	}
}

void LogMessage(const std::wstring& message)
{
	if (!g_hEditLog) return;

	int len = GetWindowTextLengthW(g_hEditLog);
	SendMessageW(g_hEditLog, EM_SETSEL, len, len);
	SendMessageW(g_hEditLog, EM_REPLACESEL, FALSE, (LPARAM)(message + L"\r\n").c_str());
}

std::wstring GetAxisName()
{
	if (SendMessage(g_hRadioX, BM_GETCHECK, 0, 0) == BST_CHECKED)
		return L"X";
	else if (SendMessage(g_hRadioY, BM_GETCHECK, 0, 0) == BST_CHECKED)
		return L"Y";
	else
		return L"Z";
}

std::wstring FormatResizeFactor(double factor)
{
	std::wostringstream oss;
	oss << std::fixed << std::setprecision(2) << factor;
	std::wstring result = oss.str();

	// Replace decimal point with underscore
	size_t pos = result.find(L'.');
	if (pos != std::wstring::npos)
	{
		result[pos] = L'_';
	}

	return result;
}

void ProcessFBXFiles(HWND hWnd)
{
	// Get input values
	wchar_t sdkPath[MAX_PATH];
	wchar_t fbxFolder[MAX_PATH];
	wchar_t resizeFactorText[32];

	GetWindowTextW(g_hEditSdkPath, sdkPath, MAX_PATH);
	GetWindowTextW(g_hEditFbxFolder, fbxFolder, MAX_PATH);
	GetWindowTextW(g_hEditResizeFactor, resizeFactorText, 32);

	// Validate inputs
	if (wcslen(sdkPath) == 0 || wcslen(fbxFolder) == 0)
	{
		MessageBoxW(hWnd, L"Please fill in all required fields.", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	// Parse resize factor
	double resizeFactor = 2.0;
	try
	{
		resizeFactor = std::stod(resizeFactorText);
		if (resizeFactor <= 0)
		{
			MessageBoxW(hWnd, L"Resize factor must be greater than 0.", L"Error", MB_OK | MB_ICONERROR);
			return;
		}
	}
	catch (...)
	{
		MessageBoxW(hWnd, L"Invalid resize factor. Please enter a valid number.", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	// Clear log
	SetWindowTextW(g_hEditLog, L"");

	// Get axis and formatted factor
	std::wstring axis = GetAxisName();
	std::wstring formattedFactor = FormatResizeFactor(resizeFactor);

	// Determine resize axis enum
	FbxProcessor::ResizeAxis resizeAxis = FbxProcessor::ResizeAxis::Y;
	if (axis == L"X")
		resizeAxis = FbxProcessor::ResizeAxis::X;
	else if (axis == L"Z")
		resizeAxis = FbxProcessor::ResizeAxis::Z;

	LogMessage(L"=== Starting FBX Processing ===");
	LogMessage(L"SDK Path: " + std::wstring(sdkPath));
	LogMessage(L"FBX Folder: " + std::wstring(fbxFolder));
	LogMessage(L"Resize Axis: " + axis);
	LogMessage(L"Resize Factor: " + std::to_wstring(resizeFactor));
	LogMessage(L"");

	// Check if folder exists
	if (!fs::exists(fbxFolder))
	{
		LogMessage(L"ERROR: FBX folder does not exist!");
		MessageBoxW(hWnd, L"FBX folder does not exist.", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	// Initialize FBX SDK
	LogMessage(L"Initializing FBX SDK...");
	if (!FbxProcessor::InitializeFbxSdk(sdkPath))
	{
		std::wstring error = L"Failed to initialize FBX SDK: " + FbxProcessor::GetLastError();
		LogMessage(error);
		MessageBoxW(hWnd, error.c_str(), L"Error", MB_OK | MB_ICONERROR);
		return;
	}
	LogMessage(L"FBX SDK initialized successfully");
	LogMessage(L"");

	// Process FBX files
	int processedCount = 0;
	int successCount = 0;
	int skippedCount = 0;
	try
	{
		for (const auto& entry : fs::directory_iterator(fbxFolder))
		{
			if (entry.is_regular_file() && entry.path().extension() == L".fbx")
			{
				std::wstring filename = entry.path().stem().wstring();

				// Skip files that already have "_Resized_" in their name
				if (filename.find(L"_Resized_") != std::wstring::npos)
				{
					LogMessage(L"Skipping (already resized): " + filename + L".fbx");
					skippedCount++;
					continue;
				}

				std::wstring outputFilename = filename + L"_Resized_" + axis + L"_" + formattedFactor + L".fbx";
				std::wstring outputPath = entry.path().parent_path().wstring() + L"\\" + outputFilename;

				LogMessage(L"Processing: " + filename + L".fbx");

				// Process the FBX file
				FbxProcessor::ProcessResult result = FbxProcessor::ProcessFbxFile(
					entry.path().wstring(),
					outputPath,
					resizeAxis,
					resizeFactor);

				if (result.success)
				{
					LogMessage(L"  ✓ Output: " + outputFilename);
					successCount++;
				}
				else
				{
					LogMessage(L"  ✗ Error: " + result.message);
				}

				processedCount++;
			}
		}

		LogMessage(L"");
		LogMessage(L"=== Processing Complete ===");
		LogMessage(L"Total files processed: " + std::to_wstring(processedCount));
		LogMessage(L"Successful: " + std::to_wstring(successCount));
		LogMessage(L"Failed: " + std::to_wstring(processedCount - successCount));
		LogMessage(L"Skipped (already resized): " + std::to_wstring(skippedCount));

		// Cleanup FBX SDK
		FbxProcessor::CleanupFbxSdk();

		std::wstring message = L"Processing complete!\n\nFiles processed: " + 
			std::to_wstring(processedCount) + L"\nSuccessful: " + 
			std::to_wstring(successCount) + L"\nFailed: " + 
			std::to_wstring(processedCount - successCount) + L"\nSkipped: " + 
			std::to_wstring(skippedCount);

		MessageBoxW(hWnd, message.c_str(), L"Success", MB_OK | MB_ICONINFORMATION);
	}
	catch (const std::exception& e)
	{
		std::string errorMsg = e.what();
		std::wstring wErrorMsg(errorMsg.begin(), errorMsg.end());
		LogMessage(L"ERROR: " + wErrorMsg);

		FbxProcessor::CleanupFbxSdk();

		MessageBoxW(hWnd, L"An error occurred during processing.", L"Error", MB_OK | MB_ICONERROR);
	}
}
