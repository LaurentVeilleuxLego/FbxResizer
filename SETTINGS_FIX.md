# Settings Save/Load Fix - Preventing Circular Updates

## Issue Fixed

**Problem:** Settings were not persisting between application restarts. Values always reset to defaults.

**Root Cause:** When `LoadSettings()` called `SetWindowTextW()` to restore saved values, it triggered `EN_CHANGE` events for each text field. These events immediately called `SaveSettings()`, which overwrote the loaded values with the defaults that were initially set in the control creation.

## The Circular Problem

### What Was Happening

```
1. Application starts
2. CreateControls() creates controls with default values
3. LoadSettings() tries to load saved values
4. SetWindowTextW() sets loaded values ? Triggers EN_CHANGE event
5. EN_CHANGE handler calls SaveSettings()
6. SaveSettings() reads current text (which might be default)
7. Saves defaults back to registry
8. Loop continues for each field
Result: Saved values overwritten with defaults!
```

### Visual Timeline

```
Time  Action                           Registry Value
????????????????????????????????????????????????????
T0    User sets "3.5" and closes      ResizeFactor=3.5
T1    App restarts
T2    Control created with "2.00"     ResizeFactor=3.5
T3    LoadSettings reads "3.5"        ResizeFactor=3.5
T4    SetWindowText("3.5")            ResizeFactor=3.5
T5    EN_CHANGE event fires           ResizeFactor=3.5
T6    SaveSettings() called           ResizeFactor=3.5
T7    Reads current text "3.5"        ResizeFactor=3.5
T8    Saves "3.5"                     ResizeFactor=3.5  ? (unwanted save)

But sometimes timing causes:
T2    Control created with "2.00"     ResizeFactor=3.5
T3    EN_CHANGE fires (from create)   ResizeFactor=3.5
T4    SaveSettings saves "2.00"       ResizeFactor=2.00  ? (overwrites saved!)
T5    LoadSettings tries to load      ResizeFactor=2.00
T6    But registry now has "2.00"     ResizeFactor=2.00
```

## The Solution

Added a flag `g_bLoadingSettings` to prevent saving during load:

### Implementation

```cpp
// Global flag
bool g_bLoadingSettings = false;

void LoadSettings()
{
    // Set flag to prevent SaveSettings during load
    g_bLoadingSettings = true;

    // Load and set all values
    SetWindowTextW(g_hEditSdkPath, savedValue);  // Triggers EN_CHANGE
    SetWindowTextW(g_hEditFbxFolder, savedValue); // Triggers EN_CHANGE
    // ... etc

    // Clear flag - now user changes will be saved
    g_bLoadingSettings = false;
}

void SaveSettings()
{
    // Don't save if we're currently loading
    if (g_bLoadingSettings)
        return;

    // Save all values
    ...
}
```

### New Timeline (Fixed)

```
Time  Action                           Registry Value
????????????????????????????????????????????????????
T0    User sets "3.5" and closes      ResizeFactor=3.5
T1    App restarts
T2    Control created with "2.00"     ResizeFactor=3.5
T3    g_bLoadingSettings = true       ResizeFactor=3.5
T4    LoadSettings reads "3.5"        ResizeFactor=3.5
T5    SetWindowText("3.5")            ResizeFactor=3.5
T6    EN_CHANGE event fires           ResizeFactor=3.5
T7    SaveSettings() called           ResizeFactor=3.5
T8    Checks flag - returns early     ResizeFactor=3.5  ? (no save)
T9    g_bLoadingSettings = false      ResizeFactor=3.5
T10   User can now make changes       ResizeFactor=3.5
```

## How It Works Now

### Startup Sequence

1. ? Controls created with default values
2. ? `g_bLoadingSettings = true` (flag set)
3. ? LoadSettings() reads registry
4. ? SetWindowText() sets saved values
5. ? EN_CHANGE events fire
6. ? SaveSettings() sees flag and returns early (no save)
7. ? All fields loaded
8. ? `g_bLoadingSettings = false` (flag cleared)
9. ? Ready for user input

### User Makes Changes

1. ? User types in field
2. ? EN_CHANGE event fires
3. ? SaveSettings() checks flag (false)
4. ? Proceeds to save
5. ? Value saved to registry

## Benefits

? **Settings persist correctly** - Saved values are restored  
? **No circular updates** - Loading doesn't trigger saving  
? **User changes still saved** - Flag only active during load  
? **Clean implementation** - Simple boolean flag  

## Testing

### Test 1: Basic Persistence
```
1. Set Resize Factor to "3.5"
2. Close application
3. Reopen application
Expected: "3.5" appears in field ?
```

### Test 2: Multiple Fields
```
1. Change SDK path, FBX folder, resize factor
2. Close application
3. Reopen application
Expected: All values restored ?
```

### Test 3: User Changes Still Save
```
1. Open application
2. Change a value
3. Close application
4. Reopen application
Expected: New value appears ?
```

### Test 4: Axis Selection
```
1. Select Z-Axis
2. Close application
3. Reopen application
Expected: Z-Axis still selected ?
```

## Code Changes Summary

### Added Global Flag
```cpp
bool g_bLoadingSettings = false;
```

### Modified LoadSettings()
```cpp
void LoadSettings()
{
    g_bLoadingSettings = true;  // ? Set flag

    // Load all settings...

    g_bLoadingSettings = false; // ? Clear flag
}
```

### Modified SaveSettings()
```cpp
void SaveSettings()
{
    if (g_bLoadingSettings)    // ? Check flag
        return;                 // ? Early return if loading

    // Save all settings...
}
```

## Why This Pattern Works

This is a common pattern for preventing event feedback loops:

1. **Set flag before operation** - Signals "special mode"
2. **Perform operation** - May trigger events
3. **Event handlers check flag** - Skip processing if in special mode
4. **Clear flag after operation** - Return to normal mode

Used in many scenarios:
- Loading data (this case)
- Programmatic updates
- Batch operations
- Cascading updates prevention

## Technical Notes

### Thread Safety
Not needed - all operations on UI thread.

### Performance
Negligible - simple boolean check.

### Maintainability
If adding new fields:
1. Add to LoadSettings() (between flag set/clear)
2. Add to SaveSettings() (already protected)
3. No other changes needed

## Alternative Solutions Considered

### Alternative 1: Delay Loading
```cpp
// Load settings after a timer
SetTimer(hWnd, LOAD_TIMER, 100, NULL);
```
**Rejected:** Introduces timing issues, flicker

### Alternative 2: Subclass Controls
```cpp
// Override EN_CHANGE in custom control
```
**Rejected:** More complex, harder to maintain

### Alternative 3: Block Events
```cpp
// Temporarily remove event handlers
```
**Rejected:** Windows API doesn't support this cleanly

### Chosen Solution: Flag Check ?
Simple, reliable, maintainable

## Summary

**Problem:** Settings not persisting due to circular save/load updates  
**Solution:** Added `g_bLoadingSettings` flag to prevent saving during load  
**Result:** Settings now correctly persist between application sessions  

**Build successful** - Restart and your settings will now be saved and restored! ??
