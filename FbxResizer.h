// FbxResizer.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <shlobj.h>
#include <filesystem>
#include <sstream>
#include <iomanip>

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Control IDs
#define IDC_EDIT_SDK_PATH           1001
#define IDC_BTN_BROWSE_SDK          1002
#define IDC_EDIT_FBX_FOLDER         1003
#define IDC_BTN_BROWSE_FBX          1004
#define IDC_LISTBOX_CONFIGS         1005
#define IDC_COMBO_AXIS              1006
#define IDC_EDIT_FACTOR             1007
#define IDC_BTN_ADD_CONFIG          1008
#define IDC_BTN_REMOVE_CONFIG       1009
#define IDC_BTN_PROCESS             1010
#define IDC_EDIT_LOG                1011
#define IDC_EDIT_METADATA_X         1012
#define IDC_EDIT_METADATA_Y         1013
#define IDC_EDIT_METADATA_Z         1014
#define IDC_BTN_RESET               1015
#define IDC_EDIT_SKIP_STRING        1016

// Window dimensions
#define WINDOW_WIDTH                700
#define WINDOW_HEIGHT               920
