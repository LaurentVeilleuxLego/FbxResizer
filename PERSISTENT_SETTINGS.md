# Persistent Settings - Auto-Save & Auto-Load

## What's New

All input fields are now automatically saved when changed and restored when the application is reopened!

## How It Works

### Automatic Saving

Settings are saved **immediately** when you:
- ? Type in any text field
- ? Browse and select a folder
- ? Change the resize factor
- ? Select a different axis (X, Y, Z)
- ? Modify any metadata property list

**No manual save button needed!**

### Automatic Loading

When you launch the application:
- ? All previous values are automatically restored
- ? Last used SDK path
- ? Last used FBX folder
- ? Last resize factor
- ? Last selected axis
- ? All metadata property lists

**Pick up right where you left off!**

## Saved Settings

### All Input Fields
```
? FBX SDK DLL Folder path
? FBX Folder path
? Resize Factor
? Selected Axis (X, Y, or Z)
? X-Axis metadata properties
? Y-Axis metadata properties
? Z-Axis metadata properties
```

## Storage Location

Settings are stored in the **Windows Registry** at:
```
HKEY_CURRENT_USER\Software\FBXResizer
```

### Registry Entries
- `SdkPath` - SDK folder path
- `FbxFolder` - FBX files folder path
- `ResizeFactor` - Resize multiplier (e.g., "2.00")
- `SelectedAxis` - 0=X, 1=Y, 2=Z
- `MetadataX` - X-axis property names
- `MetadataY` - Y-axis property names
- `MetadataZ` - Z-axis property names

## Usage Examples

### Example 1: Daily Workflow

**Day 1:**
```
1. Open application
2. Configure:
   - SDK Path: C:\FBX SDK\...
   - FBX Folder: C:\MyProject\Models
   - Y-Axis properties: height, y
   - Resize Factor: 2.0
3. Process files
4. Close application
```

**Day 2:**
```
1. Open application
2. All settings from Day 1 are already there! ?
3. Just click "Process FBX Files"
```

### Example 2: Switching Between Projects

**Morning - Project A:**
```
Settings:
  FBX Folder: C:\ProjectA\Models
  Y-Axis: height, leg_length
  Factor: 2.0

Process files ? Settings saved
```

**Afternoon - Project B:**
```
Change settings:
  FBX Folder: C:\ProjectB\Characters
  Y-Axis: body_height, arm_length
  Factor: 1.5

Process files ? Settings saved
Close application
```

**Next Day:**
```
Open application
? ProjectB settings are loaded (last used)
```

### Example 3: Testing Different Factors

```
Session 1: Factor 1.5 ? Saved
Session 2: Factor 2.0 ? Saved
Session 3: Factor 2.5 ? Saved

Each time you reopen, the last factor is remembered
```

## Benefits

? **Convenience** - No need to re-enter paths every time  
? **Productivity** - Start working immediately  
? **Safety** - Never lose your configuration  
? **Flexibility** - Easy to switch between projects  
? **Smart** - Remembers everything automatically  

## Default Values (First Launch)

When you run the application for the first time:

```
SDK Path: C:\Program Files\Autodesk\FBX\FBX SDK\2020.3.4\lib\vs2021\x64\release
FBX Folder: C:\Git\Atom2\TestProjects\Play_Atom\Content\VAD\ModularSetup
Resize Factor: 2.00
Selected Axis: Y
X-Axis Properties: width, x
Y-Axis Properties: height, y
Z-Axis Properties: depth, length, z
```

After you change any value, your changes are remembered.

## When Settings Are Saved

### Text Field Changes
```
Type in field ? EN_CHANGE event ? Save settings
```

Example:
- Type "3.0" in Resize Factor
- Immediately saved (no delay)

### Radio Button Changes
```
Click different axis ? BN_CLICKED event ? Save settings
```

Example:
- Click "Z Axis"
- Immediately saved

### Folder Browser
```
Select folder ? OK clicked ? Save settings
```

Example:
- Browse to new folder
- Click OK
- Settings saved with new path

## When Settings Are Loaded

Settings are loaded once when the application starts:

```
Application Launch
  ?
CreateControls() creates UI elements
  ?
LoadSettings() restores saved values
  ?
UI ready with your previous settings
```

## Technical Details

### Save Implementation
```cpp
void SaveSettings()
{
    // Get current values from UI
    // Write to registry
    // Done immediately
}
```

### Load Implementation
```cpp
void LoadSettings()
{
    // Read from registry
    // Set UI control values
    // Called once at startup
}
```

### Registry Operations
- Uses standard Windows Registry API
- `RegCreateKeyEx` / `RegOpenKeyEx`
- `RegSetValueEx` / `RegQueryValueEx`
- User-specific settings (HKEY_CURRENT_USER)
- No admin rights required

## Privacy & Security

### What's Stored
- ? File paths (local to your machine)
- ? Configuration values
- ? No file contents
- ? No personal data
- ? No internet connection

### Storage Security
- Settings stored in your user profile
- Other users can't see your settings
- Standard Windows Registry security
- Can be cleared via Registry Editor if desired

## Resetting to Defaults

### Method 1: Manual Registry Edit
1. Press Win+R
2. Type `regedit`
3. Navigate to `HKEY_CURRENT_USER\Software\FBXResizer`
4. Delete the key
5. Restart application ? defaults restored

### Method 2: Overwrite Values
Simply change the values in the UI:
- They'll be saved immediately
- New values become your new defaults

## Troubleshooting

### "Settings not saving"

**Check:**
1. Do you have write access to registry?
2. Is the application running normally?
3. Are you typing in the correct fields?

**Solution:** Settings save automatically. If they're not saving, restart the application as administrator once.

### "Settings not loading"

**Check:**
1. Is this the first time running?
2. Did you clear the registry?

**Solution:** Normal - defaults are used on first run. Change values to start saving.

### "Old settings still appear"

**Cause:** Settings persist across runs (by design!)

**Solution:** Change the values in the UI - new values will be saved and used next time.

## Use Cases

### Use Case 1: Batch Processing Daily
```
Day 1: Set up everything
Day 2-30: Open app ? Process ? Close
No reconfiguration needed!
```

### Use Case 2: Multiple Artists
```
Artist A: Has their own settings
Artist B: Has their own settings
Each person sees their own configuration
```

### Use Case 3: Testing & Iteration
```
Try different factors easily:
1. Open app
2. Change factor from 2.0 to 2.5
3. Process
4. Close
5. Reopen ? 2.5 is still there
```

### Use Case 4: Project Templates
```
Project 1 setup:
- Configure once
- Use for weeks
- Settings always ready

Project 2 setup:
- Change configuration
- New settings saved
- Use until project ends
```

## Comparison

### Before (No Persistence)
```
Every launch:
1. Type SDK path ?
2. Browse to folder ?
3. Set resize factor ?
4. Configure metadata ?
5. Select axis ?
6. Finally process ?

Time wasted: ~2 minutes per launch
```

### After (Auto-Save)
```
Every launch:
1. Click "Process FBX Files" ?

Time saved: ~2 minutes per launch
```

## Advanced: Manual Registry Access

If you want to backup or share settings:

### Export Settings
```
1. Win+R ? regedit
2. Navigate to HKEY_CURRENT_USER\Software\FBXResizer
3. Right-click key ? Export
4. Save .reg file
```

### Import Settings
```
1. Double-click .reg file
2. Confirm import
3. Restart application
```

This allows sharing configurations between computers!

## Summary

**What Changed:**
- All input fields now save automatically
- Settings load automatically on startup
- No manual save/load buttons needed

**Benefits:**
- ? Instant productivity
- ? No data loss
- ? Seamless experience
- ? Per-user settings

**Storage:**
- Windows Registry
- HKEY_CURRENT_USER\Software\FBXResizer
- User-specific, secure

**Build successful** - Restart and enjoy auto-save/load! ??
