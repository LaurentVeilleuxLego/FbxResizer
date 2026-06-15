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
HWND g_hEditMetadataX = NULL;
HWND g_hEditMetadataY = NULL;
HWND g_hEditMetadataZ = NULL;
HWND g_hEditLog = NULL;
HFONT g_hFont = NULL;
bool g_bLoadingSettings = false; // Flag to prevent saving during load

// Forward declarations
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void CreateControls(HWND hWnd);
void BrowseForFolder(HWND hWnd, HWND hEdit, const wchar_t* title);
void ProcessFBXFiles(HWND hWnd);
void LogMessage(const std::wstring& message);
std::wstring GetAxisName();
std::wstring FormatResizeFactor(double factor);
void SaveSettings();
void LoadSettings();
void ResetSettings();
std::wstring GetRegistryString(const wchar_t* valueName, const wchar_t* defaultValue);
void SetRegistryString(const wchar_t* valueName, const wchar_t* value);
int GetRegistryInt(const wchar_t* valueName, int defaultValue);
void SetRegistryInt(const wchar_t* valueName, int value);

// Registry key for settings
const wchar_t* REG_KEY_PATH = L"Software\\FBXResizer";

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
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
	wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
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
		L"FBX RESIZER",
		WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		MessageBoxW(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Explicitly set window title to ensure it displays correctly
	SetWindowTextW(hWnd, L"FBX RESIZER");

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
		SetWindowTextW(hWnd, L"FBX RESIZER");
		g_bLoadingSettings = true; // Set flag before creating controls
		CreateControls(hWnd);
		break;

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);

		switch (wmId)
		{
		case IDC_BTN_BROWSE_SDK:
			BrowseForFolder(hWnd, g_hEditSdkPath, L"Select FBX SDK DLL Folder");
			SaveSettings(); // Save after browse
			break;

		case IDC_BTN_BROWSE_FBX:
			BrowseForFolder(hWnd, g_hEditFbxFolder, L"Select FBX Folder");
			SaveSettings(); // Save after browse
			break;

		case IDC_EDIT_SDK_PATH:
		case IDC_EDIT_FBX_FOLDER:
		case IDC_EDIT_RESIZE_FACTOR:
		case IDC_EDIT_METADATA_X:
		case IDC_EDIT_METADATA_Y:
		case IDC_EDIT_METADATA_Z:
			// Save on text change
			if (wmEvent == EN_CHANGE)
			{
				SaveSettings();
			}
			break;

		case IDC_RADIO_X:
		case IDC_RADIO_Y:
		case IDC_RADIO_Z:
			// Save on radio button click
			if (wmEvent == BN_CLICKED)
			{
				SaveSettings();
			}
			break;

		case IDC_BTN_PROCESS:
			ProcessFBXFiles(hWnd);
			break;

		case IDC_BTN_RESET:
			if (MessageBoxW(hWnd, L"Are you sure you want to reset all settings to default values?", 
				L"Confirm Reset", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				ResetSettings();
			}
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
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		140, yPos, 120, 25,
		hWnd, (HMENU)IDC_EDIT_RESIZE_FACTOR, NULL, NULL);
	SendMessage(g_hEditResizeFactor, WM_SETFONT, (WPARAM)g_hFont, TRUE);

	CreateWindowW(L"STATIC", L"Example: 2.0 = double size, 0.5 = half size",
		WS_VISIBLE | WS_CHILD,
		270, yPos + 3, 350, 20,
		hWnd, NULL, NULL, NULL);
	yPos += 40;

	// Metadata Property Names section
	CreateWindowW(L"STATIC", L"Metadata Property Names to Scale (comma-separated, per axis)",
		WS_VISIBLE | WS_CHILD,
		15, yPos, 650, 20,
		hWnd, NULL, NULL, NULL);
	yPos += 25;

	// X-Axis metadata
	CreateWindowW(L"STATIC", L"X-Axis Properties:",
		WS_VISIBLE | WS_CHILD,
		15, yPos + 3, 120, 20,
		hWnd, NULL, NULL, NULL);

	g_hEditMetadataX = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT",
		L"width, x",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		140, yPos, 510, 25,
		hWnd, (HMENU)IDC_EDIT_METADATA_X, NULL, NULL);
	SendMessage(g_hEditMetadataX, WM_SETFONT, (WPARAM)g_hFont, TRUE);
	yPos += 30;

	// Y-Axis metadata
	CreateWindowW(L"STATIC", L"Y-Axis Properties:",
		WS_VISIBLE | WS_CHILD,
		15, yPos + 3, 120, 20,
		hWnd, NULL, NULL, NULL);

	g_hEditMetadataY = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT",
		L"height, y",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		140, yPos, 510, 25,
		hWnd, (HMENU)IDC_EDIT_METADATA_Y, NULL, NULL);
	SendMessage(g_hEditMetadataY, WM_SETFONT, (WPARAM)g_hFont, TRUE);
	yPos += 30;

	// Z-Axis metadata
	CreateWindowW(L"STATIC", L"Z-Axis Properties:",
		WS_VISIBLE | WS_CHILD,
		15, yPos + 3, 120, 20,
		hWnd, NULL, NULL, NULL);

	g_hEditMetadataZ = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT",
		L"depth, length, z",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		140, yPos, 510, 25,
		hWnd, (HMENU)IDC_EDIT_METADATA_Z, NULL, NULL);
	SendMessage(g_hEditMetadataZ, WM_SETFONT, (WPARAM)g_hFont, TRUE);

	CreateWindowW(L"STATIC", L"Tip: Property names are case-insensitive. Only properties for the selected axis will be scaled.",
		WS_VISIBLE | WS_CHILD,
		15, yPos + 30, 650, 20,
		hWnd, NULL, NULL, NULL);
	yPos += 60;

	// Process button
	CreateWindowW(L"BUTTON", L"Process FBX Files",
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		15, yPos, 520, 35,
		hWnd, (HMENU)IDC_BTN_PROCESS, NULL, NULL);

	// Reset button
	CreateWindowW(L"BUTTON", L"Reset Settings",
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		545, yPos, 120, 35,
		hWnd, (HMENU)IDC_BTN_RESET, NULL, NULL);
	yPos += 50;

	// Processing Log section
	CreateWindowW(L"STATIC", L"Processing Log:",
		WS_VISIBLE | WS_CHILD,
		15, yPos, 650, 20,
		hWnd, NULL, NULL, NULL);
	yPos += 25;

	g_hEditLog = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
		WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
		15, yPos, 650, 150,
		hWnd, (HMENU)IDC_EDIT_LOG, NULL, NULL);
	SendMessage(g_hEditLog, WM_SETFONT, (WPARAM)g_hFont, TRUE);

	// Load saved settings
	LoadSettings();
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
	wchar_t metadataXText[1024];
	wchar_t metadataYText[1024];
	wchar_t metadataZText[1024];

	GetWindowTextW(g_hEditSdkPath, sdkPath, MAX_PATH);
	GetWindowTextW(g_hEditFbxFolder, fbxFolder, MAX_PATH);
	GetWindowTextW(g_hEditResizeFactor, resizeFactorText, 32);
	GetWindowTextW(g_hEditMetadataX, metadataXText, 1024);
	GetWindowTextW(g_hEditMetadataY, metadataYText, 1024);
	GetWindowTextW(g_hEditMetadataZ, metadataZText, 1024);

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

	// Determine resize axis enum and select appropriate metadata property list
	FbxProcessor::ResizeAxis resizeAxis = FbxProcessor::ResizeAxis::Y;
	std::wstring metadataText;

	if (axis == L"X")
	{
		resizeAxis = FbxProcessor::ResizeAxis::X;
		metadataText = metadataXText;
	}
	else if (axis == L"Z")
	{
		resizeAxis = FbxProcessor::ResizeAxis::Z;
		metadataText = metadataZText;
	}
	else // Y axis
	{
		resizeAxis = FbxProcessor::ResizeAxis::Y;
		metadataText = metadataYText;
	}

	// Parse metadata property names for the selected axis (comma-separated)
	std::vector<std::wstring> metadataPropertyNames;
	std::wstringstream ss(metadataText);
	std::wstring token;
	while (std::getline(ss, token, L','))
	{
		// Trim whitespace
		size_t start = token.find_first_not_of(L" \t");
		size_t end = token.find_last_not_of(L" \t");
		if (start != std::wstring::npos && end != std::wstring::npos)
		{
			metadataPropertyNames.push_back(token.substr(start, end - start + 1));
		}
	}

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

				// Skip files that already have "_Resized_" in their name to prevent cascading
				if (filename.find(L"_Resized_") != std::wstring::npos)
				{
					skippedCount++;
					continue;
				}

				std::wstring outputFilename = filename + L"_Resized_" + axis + L"_" + formattedFactor + L".fbx";
				std::wstring outputPath = entry.path().parent_path().wstring() + L"\\" + outputFilename;

				// Check if output file already exists and will be overwritten
				if (fs::exists(outputPath))
				{
					LogMessage(L"Overwriting: " + outputFilename);
				}

				LogMessage(L"Processing: " + filename + L".fbx");

				// Process the FBX file
				FbxProcessor::ProcessResult result = FbxProcessor::ProcessFbxFile(
					entry.path().wstring(),
					outputPath,
					resizeAxis,
					resizeFactor,
					metadataPropertyNames);

				if (result.success)
				{
					LogMessage(L"  ✓ Output: " + outputFilename);

					// Log metadata scaling if any occurred
					for (const auto& metadataLog : result.metadataLogs)
					{
						LogMessage(metadataLog);
					}

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
		if (skippedCount > 0)
		{
			LogMessage(L"Skipped (already resized): " + std::to_wstring(skippedCount));
		}

		// Cleanup FBX SDK
		FbxProcessor::CleanupFbxSdk();

		std::wstring message = L"Processing complete!\n\nFiles processed: " + 
			std::to_wstring(processedCount) + L"\nSuccessful: " + 
			std::to_wstring(successCount) + L"\nFailed: " + 
			std::to_wstring(processedCount - successCount);

		if (skippedCount > 0)
		{
			message += L"\nSkipped: " + std::to_wstring(skippedCount);
		}

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

// Registry helper functions
std::wstring GetRegistryString(const wchar_t* valueName, const wchar_t* defaultValue)
{
	HKEY hKey;
	if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_KEY_PATH, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return defaultValue;
	}

	wchar_t buffer[2048];
	DWORD bufferSize = sizeof(buffer);
	DWORD type = REG_SZ;

	if (RegQueryValueExW(hKey, valueName, NULL, &type, (LPBYTE)buffer, &bufferSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return defaultValue;
	}

	RegCloseKey(hKey);
	return std::wstring(buffer);
}

void SetRegistryString(const wchar_t* valueName, const wchar_t* value)
{
	HKEY hKey;
	if (RegCreateKeyExW(HKEY_CURRENT_USER, REG_KEY_PATH, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
	{
		return;
	}

	RegSetValueExW(hKey, valueName, 0, REG_SZ, (LPBYTE)value, (DWORD)((wcslen(value) + 1) * sizeof(wchar_t)));
	RegCloseKey(hKey);
}

int GetRegistryInt(const wchar_t* valueName, int defaultValue)
{
	HKEY hKey;
	if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_KEY_PATH, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return defaultValue;
	}

	DWORD value = defaultValue;
	DWORD bufferSize = sizeof(DWORD);
	DWORD type = REG_DWORD;

	if (RegQueryValueExW(hKey, valueName, NULL, &type, (LPBYTE)&value, &bufferSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return defaultValue;
	}

	RegCloseKey(hKey);
	return (int)value;
}

void SetRegistryInt(const wchar_t* valueName, int value)
{
	HKEY hKey;
	if (RegCreateKeyExW(HKEY_CURRENT_USER, REG_KEY_PATH, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
	{
		return;
	}

	DWORD dwValue = (DWORD)value;
	RegSetValueExW(hKey, valueName, 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
	RegCloseKey(hKey);
}

void SaveSettings()
{
	// Don't save if we're currently loading settings
	if (g_bLoadingSettings)
		return;

	wchar_t buffer[2048];

	// Save text fields
	GetWindowTextW(g_hEditSdkPath, buffer, 2048);
	SetRegistryString(L"SdkPath", buffer);

	GetWindowTextW(g_hEditFbxFolder, buffer, 2048);
	SetRegistryString(L"FbxFolder", buffer);

	GetWindowTextW(g_hEditResizeFactor, buffer, 2048);
	SetRegistryString(L"ResizeFactor", buffer);

	GetWindowTextW(g_hEditMetadataX, buffer, 2048);
	SetRegistryString(L"MetadataX", buffer);

	GetWindowTextW(g_hEditMetadataY, buffer, 2048);
	SetRegistryString(L"MetadataY", buffer);

	GetWindowTextW(g_hEditMetadataZ, buffer, 2048);
	SetRegistryString(L"MetadataZ", buffer);

	// Save selected axis
	int selectedAxis = 1; // Default Y
	if (SendMessage(g_hRadioX, BM_GETCHECK, 0, 0) == BST_CHECKED)
		selectedAxis = 0;
	else if (SendMessage(g_hRadioZ, BM_GETCHECK, 0, 0) == BST_CHECKED)
		selectedAxis = 2;
	SetRegistryInt(L"SelectedAxis", selectedAxis);
}

void LoadSettings()
{
	// Set flag to prevent SaveSettings from being called during load
	g_bLoadingSettings = true;

	// Load text fields
	std::wstring sdkPath = GetRegistryString(L"SdkPath", L"C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.3.4\\lib\\vs2021\\x64\\release");
	SetWindowTextW(g_hEditSdkPath, sdkPath.c_str());

	std::wstring fbxFolder = GetRegistryString(L"FbxFolder", L"C:\\Git\\Atom2\\TestProjects\\Play_Atom\\Content\\VAD\\ModularSetup");
	SetWindowTextW(g_hEditFbxFolder, fbxFolder.c_str());

	std::wstring resizeFactor = GetRegistryString(L"ResizeFactor", L"2.00");
	SetWindowTextW(g_hEditResizeFactor, resizeFactor.c_str());

	std::wstring metadataX = GetRegistryString(L"MetadataX", L"width, x");
	SetWindowTextW(g_hEditMetadataX, metadataX.c_str());

	std::wstring metadataY = GetRegistryString(L"MetadataY", L"height, y");
	SetWindowTextW(g_hEditMetadataY, metadataY.c_str());

	std::wstring metadataZ = GetRegistryString(L"MetadataZ", L"depth, length, z");
	SetWindowTextW(g_hEditMetadataZ, metadataZ.c_str());

	// Load selected axis
	int selectedAxis = GetRegistryInt(L"SelectedAxis", 1); // Default Y

	// Uncheck all radio buttons first
	SendMessage(g_hRadioX, BM_SETCHECK, BST_UNCHECKED, 0);
	SendMessage(g_hRadioY, BM_SETCHECK, BST_UNCHECKED, 0);
	SendMessage(g_hRadioZ, BM_SETCHECK, BST_UNCHECKED, 0);

	// Check the selected one
	if (selectedAxis == 0)
		SendMessage(g_hRadioX, BM_SETCHECK, BST_CHECKED, 0);
	else if (selectedAxis == 2)
		SendMessage(g_hRadioZ, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(g_hRadioY, BM_SETCHECK, BST_CHECKED, 0);

	// Clear flag - now changes will be saved
	g_bLoadingSettings = false;
}

void ResetSettings()
{
	// Delete the entire registry key
	RegDeleteTreeW(HKEY_CURRENT_USER, REG_KEY_PATH);

	// Reload default settings into UI
	LoadSettings();
}
