# Custom Property Scaling - Now With Logging!

## New Feature: Metadata Scaling Log

The application now logs whenever a custom property (metadata) is scaled during processing.

## What Gets Logged

When a custom property like "height" is found and scaled, you'll see:

### Debug Output Example
```
[Metadata] Node 'leg_L_loc.001': Property 'height' scaled 1.126 -> 2.252 (axis Y, factor 2.00)
[Metadata] Node 'arm_R_loc.001': Property 'height' scaled 0.850 -> 1.700 (axis Y, factor 2.00)
[Metadata] Node 'torso_marker': Property 'Height' scaled 1.200 -> 2.400 (axis Y, factor 2.00)
```

### Information Shown
- **Node name**: Which bone/locator has the property
- **Property name**: Name of the custom property (e.g., "height")
- **Old value**: Original value before scaling
- **New value**: Value after scaling
- **Axis**: Which axis is being scaled (X, Y, or Z)
- **Factor**: The multiplication factor applied

## Where to See the Logs

### Option 1: Visual Studio Output Window (Recommended)
1. Run the application from Visual Studio (F5 or Ctrl+F5)
2. Open **View ? Output** (or press Ctrl+Alt+O)
3. Select **Debug** from the "Show output from:" dropdown
4. Process your FBX files
5. See metadata scaling logs in real-time

### Option 2: DebugView (External Tool)
1. Download **DebugView** from Microsoft Sysinternals
2. Run DebugView.exe as Administrator
3. Run FbxResizer.exe
4. Process your FBX files
5. See metadata scaling logs in DebugView window

### Option 3: Attached Debugger
If a debugger is attached, the logs appear in the debugger's output window.

## Example Processing Session

### Input File: `VX0003817_leg_L.fbx`
Contains a locator `leg_L_loc.001` with custom property `height = 1.126`

### Processing with Y-axis, Factor 2.0

**Console/UI Log:**
```
Processing: VX0003817_leg_L.fbx
  ? Output: VX0003817_leg_L_Resized_Y_2_00.fbx
```

**Debug Output Log:**
```
[Metadata] Node 'leg_L_loc.001': Property 'height' scaled 1.126 -> 2.252 (axis Y, factor 2.00)
```

## Properties That Trigger Logging

### Y-Axis Scaling
Logs when these properties are found:
- `height`, `Height`, `HEIGHT`
- `y`, `Y`

### X-Axis Scaling
Logs when these properties are found:
- `width`, `Width`, `WIDTH`
- `x`, `X`

### Z-Axis Scaling
Logs when these properties are found:
- `depth`, `Depth`, `DEPTH`
- `length`, `Length`, `LENGTH`
- `z`, `Z`

## Use Cases

### 1. Verify Custom Properties Are Being Scaled
```
Expected: height property should double
Debug Log: [Metadata] Node 'bone_001': Property 'height' scaled 1.500 -> 3.000 ?
```

### 2. Debug Missing Properties
```
No log appears? ? The property doesn't exist or has a different name
```

### 3. Track All Metadata Changes
```
Multiple nodes with height properties:
[Metadata] Node 'leg_L': Property 'height' scaled 1.126 -> 2.252
[Metadata] Node 'leg_R': Property 'height' scaled 1.126 -> 2.252
[Metadata] Node 'arm_L': Property 'height' scaled 0.850 -> 1.700
[Metadata] Node 'arm_R': Property 'height' scaled 0.850 -> 1.700
```

### 4. Verify Different Property Names Work
```
[Metadata] Node 'marker1': Property 'height' scaled 1.000 -> 2.000
[Metadata] Node 'marker2': Property 'Height' scaled 1.000 -> 2.000
[Metadata] Node 'marker3': Property 'HEIGHT' scaled 1.000 -> 2.000
```

## Technical Details

### Implementation
- Uses `OutputDebugStringA()` to send logs to debugger output
- Only logs when properties are actually scaled
- Shows both old and new values for verification
- Includes node name for context

### Performance
- Minimal overhead - only writes when properties are found
- Non-blocking - doesn't slow down processing
- Debug-only output - doesn't affect release performance

### Format
```
[Metadata] Node '<node_name>': Property '<prop_name>' scaled <old> -> <new> (axis <X/Y/Z>, factor <value>)
```

## Troubleshooting

### "I don't see any logs"

**Possible reasons:**
1. No custom properties named "height", "width", etc. exist in your FBX files
2. Properties exist but are not user-defined (system properties)
3. Properties are not numeric (float/double) type
4. Not viewing the Debug output window

**Solution:**
1. Check your FBX file in Blender/Maya for custom properties
2. Verify the property name matches (case-insensitive)
3. Ensure you're viewing the Debug output in Visual Studio

### "Too many logs"

If you process many files with lots of custom properties:
- Logs only appear in Debug output, not in the UI
- UI remains clean with just file processing status
- Debug logs can be filtered or cleared

### "Wrong values logged"

If the logged values don't match expectations:
- Check the input FBX file property values
- Verify the resize factor is correct
- Ensure you're processing the right file

## Example: Complete Processing Log

**UI Log (Main Window):**
```
=== Starting FBX Processing ===
SDK Path: C:\...\fbx\lib\x64\release
FBX Folder: C:\Models
Resize Axis: Y
Resize Factor: 2.000000

Initializing FBX SDK...
FBX SDK initialized successfully

Processing: VX0003817_leg_L.fbx
  ? Output: VX0003817_leg_L_Resized_Y_2_00.fbx

=== Processing Complete ===
Total files processed: 1
Successful: 1
Failed: 0
```

**Debug Output (Visual Studio Output Window):**
```
[Metadata] Node 'leg_L_loc.001': Property 'height' scaled 1.126 -> 2.252 (axis Y, factor 2.00)
[Metadata] Node 'leg_L_attachment': Property 'height' scaled 0.500 -> 1.000 (axis Y, factor 2.00)
[Metadata] Node 'leg_L_marker': Property 'y' scaled 1.800 -> 3.600 (axis Y, factor 2.00)
```

## Benefits

? **Transparency**: See exactly what's being modified  
? **Debugging**: Identify missing or incorrect properties  
? **Verification**: Confirm scaling math is correct  
? **Documentation**: Log serves as processing record  
? **Non-intrusive**: Doesn't clutter the main UI  

## Future Enhancements

Potential additions:
- Option to save logs to a file
- Summary of total properties scaled
- Warning for properties that couldn't be scaled
- Support for logging in the main UI window

**Build successful** - Restart and run from Visual Studio to see the logs! ??
