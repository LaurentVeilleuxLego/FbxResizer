// FbxResizer.cpp : Defines the entry point for the application.
//

#include "FbxResizer.h"
#include "FbxProcessor.h"

using namespace std;
namespace fs = std::filesystem;

// Structure to hold axis configuration
struct AxisConfig
{
	int axisIndex; // 0=X, 1=Y, 2=Z
	double factor;
	double rotation; // Rotation in degrees

	std::wstring ToString() const
	{
		const wchar_t* axisNames[] = { L"X", L"Y", L"Z" };
		std::wostringstream oss;
		oss << axisNames[axisIndex] << L" Axis - Factor: " << std::fixed << std::setprecision(2) << factor
			<< L", Rotation: " << std::fixed << std::setprecision(1) << rotation << L"°";
		return oss.str();
	}
};

// Global variables
HWND g_hEditSdkPath = NULL;
HWND g_hEditFbxFolder = NULL;
HWND g_hListBoxConfigs = NULL;
HWND g_hComboAxis = NULL;
HWND g_hEditFactor = NULL;
HWND g_hEditRotation = NULL;
HWND g_hEditMetadataX = NULL;
HWND g_hEditMetadataY = NULL;
HWND g_hEditMetadataZ = NULL;
HWND g_hEditSkipString = NULL;
HWND g_hEditLog = NULL;
HFONT g_hFont = NULL;
bool g_bLoadingSettings = false; // Flag to prevent saving during load

std::vector<AxisConfig> g_axisConfigs; // Store axis configurations

// Forward declarations
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void CreateControls(HWND hWnd);
void BrowseForFolder(HWND hWnd, HWND hEdit, const wchar_t* title);
void ProcessFBXFiles(HWND hWnd);
void LogMessage(const std::wstring& message);
std::wstring GetAxisName(int axisIndex);
FbxProcessor::ResizeAxis GetResizeAxis(int axisIndex);
std::wstring FormatResizeFactor(double factor);
std::wstring FormatRotation(double rotation);
void SaveSettings();
void LoadSettings();
void ResetSettings();
void AddAxisConfig();
void RemoveAxisConfig();
void RefreshConfigList();
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
		case IDC_EDIT_METADATA_X:
		case IDC_EDIT_METADATA_Y:
		case IDC_EDIT_METADATA_Z:
		case IDC_EDIT_SKIP_STRING:
			// Save on text change
			if (wmEvent == EN_CHANGE)
			{
				SaveSettings();
			}
			break;

		case IDC_BTN_ADD_CONFIG:
			AddAxisConfig();
			SaveSettings();
			break;

		case IDC_BTN_REMOVE_CONFIG:
			RemoveAxisConfig();
			SaveSettings();
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

	// Axis Configurations section
	CreateWindowW(L"STATIC", L"Axis, Resize Factor & Rotation Configurations",
		WS_VISIBLE | WS_CHILD,
		15, yPos, 650, 20,
		hWnd, NULL, NULL, NULL);
	yPos += 25;

	// Add configuration controls
	CreateWindowW(L"STATIC", L"Axis:",
		WS_VISIBLE | WS_CHILD,
		30, yPos + 3, 40, 20,
		hWnd, NULL, NULL, NULL);

	g_hComboAxis = CreateWindowExW(0, L"COMBOBOX", NULL,
		WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
		75, yPos, 60, 100,
		hWnd, (HMENU)IDC_COMBO_AXIS, NULL, NULL);
	SendMessage(g_hComboAxis, WM_SETFONT, (WPARAM)g_hFont, TRUE);
	SendMessageW(g_hComboAxis, CB_ADDSTRING, 0, (LPARAM)L"X");
	SendMessageW(g_hComboAxis, CB_ADDSTRING, 0, (LPARAM)L"Y");
	SendMessageW(g_hComboAxis, CB_ADDSTRING, 0, (LPARAM)L"Z");
	SendMessage(g_hComboAxis, CB_SETCURSEL, 1, 0); // Default to Y

	CreateWindowW(L"STATIC", L"Factor:",
		WS_VISIBLE | WS_CHILD,
		145, yPos + 3, 50, 20,
		hWnd, NULL, NULL, NULL);

	g_hEditFactor = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"2.00",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		200, yPos, 80, 25,
		hWnd, (HMENU)IDC_EDIT_FACTOR, NULL, NULL);
	SendMessage(g_hEditFactor, WM_SETFONT, (WPARAM)g_hFont, TRUE);

	CreateWindowW(L"STATIC", L"Rotation (\u00b0):",
		WS_VISIBLE | WS_CHILD,
		290, yPos + 3, 70, 20,
		hWnd, NULL, NULL, NULL);

	g_hEditRotation = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"0",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		365, yPos, 80, 25,
		hWnd, (HMENU)IDC_EDIT_ROTATION, NULL, NULL);
	SendMessage(g_hEditRotation, WM_SETFONT, (WPARAM)g_hFont, TRUE);

	CreateWindowW(L"BUTTON", L"+ Add",
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		455, yPos, 80, 25,
		hWnd, (HMENU)IDC_BTN_ADD_CONFIG, NULL, NULL);

	CreateWindowW(L"BUTTON", L"- Remove",
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		540, yPos, 80, 25,
		hWnd, (HMENU)IDC_BTN_REMOVE_CONFIG, NULL, NULL);

	yPos += 35;

	// List of configurations
	g_hListBoxConfigs = CreateWindowExW(WS_EX_CLIENTEDGE, L"LISTBOX", NULL,
		WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
		30, yPos, 620, 80,
		hWnd, (HMENU)IDC_LISTBOX_CONFIGS, NULL, NULL);
	SendMessage(g_hListBoxConfigs, WM_SETFONT, (WPARAM)g_hFont, TRUE);

	CreateWindowW(L"STATIC", L"Tip: Add multiple configurations. Each will create a separate output file.",
		WS_VISIBLE | WS_CHILD,
		30, yPos + 85, 620, 20,
		hWnd, NULL, NULL, NULL);
	yPos += 115;

	// Skip String section
	CreateWindowW(L"STATIC", L"Skip Files Containing (leave empty to process all files)",
		WS_VISIBLE | WS_CHILD,
		15, yPos, 650, 20,
		hWnd, NULL, NULL, NULL);
	yPos += 25;

	CreateWindowW(L"STATIC", L"Skip String:",
		WS_VISIBLE | WS_CHILD,
		30, yPos + 3, 100, 20,
		hWnd, NULL, NULL, NULL);

	g_hEditSkipString = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"_Axis_",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		140, yPos, 510, 25,
		hWnd, (HMENU)IDC_EDIT_SKIP_STRING, NULL, NULL);
	SendMessage(g_hEditSkipString, WM_SETFONT, (WPARAM)g_hFont, TRUE);

	CreateWindowW(L"STATIC", L"Example: \"_Axis_\" will skip files like \"model_Axis_Y_Scaled_2_00.fbx\"",
		WS_VISIBLE | WS_CHILD,
		30, yPos + 30, 620, 20,
		hWnd, NULL, NULL, NULL);
	yPos += 65;

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

std::wstring GetAxisName(int axisIndex)
{
	switch (axisIndex)
	{
	case 0: return L"X";
	case 1: return L"Y";
	case 2: return L"Z";
	default: return L"Y";
	}
}

FbxProcessor::ResizeAxis GetResizeAxis(int axisIndex)
{
	switch (axisIndex)
	{
	case 0: return FbxProcessor::ResizeAxis::X;
	case 1: return FbxProcessor::ResizeAxis::Y;
	case 2: return FbxProcessor::ResizeAxis::Z;
	default: return FbxProcessor::ResizeAxis::Y;
	}
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

std::wstring FormatRotation(double rotation)
{
	std::wostringstream oss;
	oss << std::fixed << std::setprecision(1) << rotation;
	std::wstring result = oss.str();

	// Replace decimal point with underscore
	size_t pos = result.find(L'.');
	if (pos != std::wstring::npos)
	{
		result[pos] = L'_';
	}

	return result;
}

void AddAxisConfig()
{
	// Get selected axis
	int axisIndex = (int)SendMessage(g_hComboAxis, CB_GETCURSEL, 0, 0);
	if (axisIndex == CB_ERR)
		axisIndex = 1; // Default to Y

	// Get factor
	wchar_t factorText[32];
	GetWindowTextW(g_hEditFactor, factorText, 32);

	// Get rotation
	wchar_t rotationText[32];
	GetWindowTextW(g_hEditRotation, rotationText, 32);

	try
	{
		double factor = std::stod(factorText);
		if (factor <= 0)
		{
			MessageBoxW(NULL, L"Factor must be greater than 0.", L"Error", MB_OK | MB_ICONERROR);
			return;
		}

		double rotation = 0.0;
		try
		{
			rotation = std::stod(rotationText);
			// Clamp rotation to -360 to 360 range
			if (rotation < -360.0 || rotation > 360.0)
			{
				MessageBoxW(NULL, L"Rotation must be between -360 and 360 degrees.", L"Error", MB_OK | MB_ICONERROR);
				return;
			}
		}
		catch (...)
		{
			MessageBoxW(NULL, L"Invalid rotation value.", L"Error", MB_OK | MB_ICONERROR);
			return;
		}

		// Add configuration
		AxisConfig config;
		config.axisIndex = axisIndex;
		config.factor = factor;
		config.rotation = rotation;
		g_axisConfigs.push_back(config);

		// Refresh list
		RefreshConfigList();
	}
	catch (...)
	{
		MessageBoxW(NULL, L"Invalid factor value.", L"Error", MB_OK | MB_ICONERROR);
	}
}

void RemoveAxisConfig()
{
	// Get selected item
	int selIndex = (int)SendMessage(g_hListBoxConfigs, LB_GETCURSEL, 0, 0);
	if (selIndex != LB_ERR && selIndex >= 0 && selIndex < (int)g_axisConfigs.size())
	{
		g_axisConfigs.erase(g_axisConfigs.begin() + selIndex);
		RefreshConfigList();
	}
}

void RefreshConfigList()
{
	// Clear list
	SendMessage(g_hListBoxConfigs, LB_RESETCONTENT, 0, 0);

	// Add all configurations
	for (const auto& config : g_axisConfigs)
	{
		std::wstring displayText = config.ToString();
		SendMessageW(g_hListBoxConfigs, LB_ADDSTRING, 0, (LPARAM)displayText.c_str());
	}
}

void ProcessFBXFiles(HWND hWnd)
{
	// Get input values
	wchar_t sdkPath[MAX_PATH];
	wchar_t fbxFolder[MAX_PATH];
	wchar_t metadataXText[1024];
	wchar_t metadataYText[1024];
	wchar_t metadataZText[1024];

	GetWindowTextW(g_hEditSdkPath, sdkPath, MAX_PATH);
	GetWindowTextW(g_hEditFbxFolder, fbxFolder, MAX_PATH);
	GetWindowTextW(g_hEditMetadataX, metadataXText, 1024);
	GetWindowTextW(g_hEditMetadataY, metadataYText, 1024);
	GetWindowTextW(g_hEditMetadataZ, metadataZText, 1024);

	// Validate inputs
	if (wcslen(sdkPath) == 0 || wcslen(fbxFolder) == 0)
	{
		MessageBoxW(hWnd, L"Please fill in all required fields.", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	// Check if we have any configurations
	if (g_axisConfigs.empty())
	{
		MessageBoxW(hWnd, L"Please add at least one axis configuration using the + Add button.", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	// Parse metadata property names for each axis
	std::vector<std::vector<std::wstring>> metadataPropertiesByAxis(3);
	std::wstring metadataTexts[] = { metadataXText, metadataYText, metadataZText };

	for (int axisIdx = 0; axisIdx < 3; ++axisIdx)
	{
		std::wstringstream ss(metadataTexts[axisIdx]);
		std::wstring token;
		while (std::getline(ss, token, L','))
		{
			// Trim whitespace
			size_t start = token.find_first_not_of(L" \t");
			size_t end = token.find_last_not_of(L" \t");
			if (start != std::wstring::npos && end != std::wstring::npos)
			{
				metadataPropertiesByAxis[axisIdx].push_back(token.substr(start, end - start + 1));
			}
		}
	}

	// Clear log
	SetWindowTextW(g_hEditLog, L"");

	LogMessage(L"=== Starting FBX Processing ===");
	LogMessage(L"SDK Path: " + std::wstring(sdkPath));
	LogMessage(L"FBX Folder: " + std::wstring(fbxFolder));
	LogMessage(L"Configurations:");
	for (const auto& config : g_axisConfigs)
	{
		LogMessage(L"  " + config.ToString());
	}
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

	// Get skip string from UI
	wchar_t skipStringBuffer[256];
	GetWindowTextW(g_hEditSkipString, skipStringBuffer, 256);
	std::wstring skipString(skipStringBuffer);

	// Process FBX files
	int processedCount = 0;
	int successCount = 0;
	int skippedCount = 0;
	int totalOutputs = 0;

	try
	{
		for (const auto& entry : fs::directory_iterator(fbxFolder))
		{
			if (entry.is_regular_file() && entry.path().extension() == L".fbx")
			{
				std::wstring filename = entry.path().stem().wstring();

				// Skip files that contain the skip string (if not empty)
				if (!skipString.empty() && filename.find(skipString) != std::wstring::npos)
				{
					skippedCount++;
					continue;
				}

				LogMessage(L"Processing: " + filename + L".fbx");
				processedCount++;

				// Process for each configuration
				for (const auto& config : g_axisConfigs)
				{
					std::wstring axis = GetAxisName(config.axisIndex);
					std::wstring formattedFactor = FormatResizeFactor(config.factor);
					std::wstring formattedRotation = FormatRotation(config.rotation);

					// Build output filename: model_Axis_Y_Scaled_2_00_Rotated_90_0.fbx
					std::wstring outputFilename = filename + L"_Axis_" + axis + L"_Scaled_" + formattedFactor;
					if (config.rotation != 0.0)
					{
						outputFilename += L"_Rotated_" + formattedRotation;
					}
					outputFilename += L".fbx";

					std::wstring outputPath = entry.path().parent_path().wstring() + L"\\" + outputFilename;

					// Check if output file already exists and will be overwritten
					if (fs::exists(outputPath))
					{
						LogMessage(L"  Overwriting: " + outputFilename);
					}

					// Get metadata properties for this axis
					const auto& metadataProps = metadataPropertiesByAxis[config.axisIndex];

					// Process the FBX file
					FbxProcessor::ProcessResult result = FbxProcessor::ProcessFbxFile(
						entry.path().wstring(),
						outputPath,
						GetResizeAxis(config.axisIndex),
						config.factor,
						config.rotation,
						metadataProps);

					if (result.success)
					{
						LogMessage(L"  ✓ " + axis + L"-axis: " + outputFilename);

						// Log metadata scaling if any occurred
						for (const auto& metadataLog : result.metadataLogs)
						{
							LogMessage(L"    " + metadataLog);
						}

						successCount++;
						totalOutputs++;
					}
					else
					{
						LogMessage(L"  ✗ " + axis + L"-axis Error: " + result.message);
					}
				}
			}
		}

		LogMessage(L"");
		LogMessage(L"=== Processing Complete ===");
		LogMessage(L"Input files processed: " + std::to_wstring(processedCount));
		LogMessage(L"Output files created: " + std::to_wstring(totalOutputs));
		LogMessage(L"Successful: " + std::to_wstring(successCount));
		LogMessage(L"Failed: " + std::to_wstring(totalOutputs - successCount));
		if (skippedCount > 0)
		{
			LogMessage(L"Skipped: " + std::to_wstring(skippedCount));
		}

		// Cleanup FBX SDK
		FbxProcessor::CleanupFbxSdk();

		std::wstring message = L"Processing complete!\n\nInput files: " + 
			std::to_wstring(processedCount) + L"\nOutput files: " + 
			std::to_wstring(totalOutputs) + L"\nSuccessful: " + 
			std::to_wstring(successCount) + L"\nFailed: " + 
			std::to_wstring(totalOutputs - successCount);

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

	GetWindowTextW(g_hEditMetadataX, buffer, 2048);
	SetRegistryString(L"MetadataX", buffer);

	GetWindowTextW(g_hEditMetadataY, buffer, 2048);
	SetRegistryString(L"MetadataY", buffer);

	GetWindowTextW(g_hEditMetadataZ, buffer, 2048);
	SetRegistryString(L"MetadataZ", buffer);

	GetWindowTextW(g_hEditSkipString, buffer, 2048);
	SetRegistryString(L"SkipString", buffer);

	// Save axis configurations count
	SetRegistryInt(L"ConfigCount", (int)g_axisConfigs.size());

	// Save each configuration
	for (size_t i = 0; i < g_axisConfigs.size(); ++i)
	{
		std::wstring axisKey = L"Config" + std::to_wstring(i) + L"_Axis";
		std::wstring factorKey = L"Config" + std::to_wstring(i) + L"_Factor";
		std::wstring rotationKey = L"Config" + std::to_wstring(i) + L"_Rotation";

		SetRegistryInt(axisKey.c_str(), g_axisConfigs[i].axisIndex);

		// Save factor as string to preserve precision
		std::wostringstream oss;
		oss << std::fixed << std::setprecision(2) << g_axisConfigs[i].factor;
		SetRegistryString(factorKey.c_str(), oss.str().c_str());

		// Save rotation as string to preserve precision
		std::wostringstream ossRot;
		ossRot << std::fixed << std::setprecision(1) << g_axisConfigs[i].rotation;
		SetRegistryString(rotationKey.c_str(), ossRot.str().c_str());
	}
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

	std::wstring metadataX = GetRegistryString(L"MetadataX", L"width, x");
	SetWindowTextW(g_hEditMetadataX, metadataX.c_str());

	std::wstring metadataY = GetRegistryString(L"MetadataY", L"height, y");
	SetWindowTextW(g_hEditMetadataY, metadataY.c_str());

	std::wstring metadataZ = GetRegistryString(L"MetadataZ", L"depth, length, z");
	SetWindowTextW(g_hEditMetadataZ, metadataZ.c_str());

	std::wstring skipString = GetRegistryString(L"SkipString", L"_Axis_");
	SetWindowTextW(g_hEditSkipString, skipString.c_str());

	// Load axis configurations
	g_axisConfigs.clear();
	int configCount = GetRegistryInt(L"ConfigCount", 1); // Default to 1 config

	if (configCount > 0)
	{
		for (int i = 0; i < configCount; ++i)
		{
			std::wstring axisKey = L"Config" + std::to_wstring(i) + L"_Axis";
			std::wstring factorKey = L"Config" + std::to_wstring(i) + L"_Factor";
			std::wstring rotationKey = L"Config" + std::to_wstring(i) + L"_Rotation";

			int axisIndex = GetRegistryInt(axisKey.c_str(), 1); // Default Y
			std::wstring factorStr = GetRegistryString(factorKey.c_str(), L"2.00");
			std::wstring rotationStr = GetRegistryString(rotationKey.c_str(), L"0");

			try
			{
				double factor = std::stod(factorStr);
				double rotation = 0.0;
				try
				{
					rotation = std::stod(rotationStr);
				}
				catch (...) {}

				AxisConfig config;
				config.axisIndex = axisIndex;
				config.factor = factor;
				config.rotation = rotation;
				g_axisConfigs.push_back(config);
			}
			catch (...) {}
		}
	}
	else
	{
		// Add default configuration if none exists
		AxisConfig config;
		config.axisIndex = 1; // Y axis
		config.factor = 2.0;
		config.rotation = 0.0;
		g_axisConfigs.push_back(config);
	}

	RefreshConfigList();

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
