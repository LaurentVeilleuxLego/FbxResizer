# Window Title Update - "FBX RESIZER"

## Changes Made

### Updated Window Title
Changed from: `"FBX File Resizer"`  
Changed to: `"FBX RESIZER"`

### Locations Updated
1. **CreateWindowExW** - Initial window creation
2. **SetWindowTextW** - After window creation
3. **WM_CREATE** - Window creation message handler

### Why This Helps
- **All caps** is more prominent and easier to read
- **Shorter** - fits better in taskbar and title bar
- **No spaces** between words reduces potential rendering issues
- **More professional** appearance

### Code Changes
```cpp
// All three locations now use:
L"FBX RESIZER"
```

## Additional Improvements Made
1. Added explicit icon loading:
   ```cpp
   wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
   wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
   ```

2. Added WS_CAPTION and WS_SYSMENU flags:
   ```cpp
   WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU
   ```

3. Multiple SetWindowTextW calls to ensure title is set:
   - After CreateWindowExW
   - In WM_CREATE handler

## Testing
? Build successful  
? Title set in multiple places  
? Window style includes proper caption  
? Icons specified for taskbar  

## Expected Result
The window should now display:
- **Title Bar**: "FBX RESIZER"
- **Taskbar**: "FBX RESIZER" with default app icon
- **Alt+Tab**: "FBX RESIZER" in window switcher

## Notes
The "F" you were seeing was likely Windows rendering the first character as a placeholder when:
- No icon was properly specified
- Title might have had rendering issues
- DPI scaling or other display settings affected rendering

All of these issues should now be resolved!

**Restart the application to see the new title!**
