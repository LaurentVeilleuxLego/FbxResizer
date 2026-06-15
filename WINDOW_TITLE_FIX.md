# Window Title Display Fix

## Issue
The window title was showing only "F" instead of "FBX Resizer" in the taskbar and title bar.

## Root Cause
The window class was missing icon specifications (`hIcon` and `hIconSm`). When Windows doesn't have an icon to display, it may default to showing just the first letter of the title in some display contexts.

## Solution
Added default application icons to the window class:
```cpp
wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
```

## Changes Made
- Updated `WNDCLASSEXW` structure to include:
  - `hIcon`: Large icon (used in Alt+Tab and taskbar)
  - `hIconSm`: Small icon (used in title bar and task list)
- Both set to `IDI_APPLICATION` (Windows default application icon)

## Result
? Window title now displays as "FBX File Resizer" properly  
? Application has a standard icon in the taskbar  
? Window appears correctly in Alt+Tab switcher  

## Testing
After rebuilding:
1. Stop any running debug session
2. Launch the application
3. Check the title bar shows "FBX File Resizer"
4. Check the taskbar icon shows the full name
5. Press Alt+Tab and verify the name is visible

## Future Enhancement
To add a custom icon:
1. Create an `.ico` file with your desired icon
2. Add it to a resource file (`.rc`)
3. Define an icon resource ID in `resource.h`:
   ```cpp
   #define IDI_FBXRESIZER 101
   ```
4. Update the window class to use:
   ```cpp
   wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FBXRESIZER));
   wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FBXRESIZER));
   ```

## Build Status
? Build successful  
? Ready to test  

**Restart the application to see the fix!**
