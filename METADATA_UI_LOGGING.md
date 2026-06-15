# Metadata Scaling Now Visible in UI Log!

## What's New

Custom property (metadata) scaling information now appears directly in the main application log window!

## What You'll See

When processing FBX files with custom properties, the log will show:

### Example UI Log Output

```
Processing: VX0003817_leg_L.fbx
  ? Output: VX0003817_leg_L_Resized_Y_2_00.fbx
    [Metadata] Node 'leg_L_loc.001': 'height' = 1.126 -> 2.252
    [Metadata] Node 'leg_L_attachment': 'height' = 0.500 -> 1.000

Processing: VX0003820_hand_R.fbx
  ? Output: VX0003820_hand_R_Resized_Y_2_00.fbx
    [Metadata] Node 'hand_R_marker': 'height' = 0.850 -> 1.700
    [Metadata] Node 'hand_R_loc': 'y' = 1.200 -> 2.400

=== Processing Complete ===
Total files processed: 2
Successful: 2
Failed: 0
```

## Information Displayed

Each metadata log line shows:
- **`[Metadata]`** - Tag to identify metadata scaling
- **`Node 'name'`** - Which bone/locator has the property
- **`'property'`** - Name of the custom property (e.g., 'height')
- **`old_value -> new_value`** - Before and after values

## Example: Complete Processing Session

### Input
- File: `VX0003817_leg_L.fbx`
- Contains locator with custom property `height = 1.126`
- Processing: Y-axis, Factor 2.0

### Output in Log Window

```
=== Starting FBX Processing ===
SDK Path: C:\Git\FbxResizer\fbx\lib\x64\release
FBX Folder: C:\MyModels
Resize Axis: Y
Resize Factor: 2.000000

Initializing FBX SDK...
FBX SDK initialized successfully

Processing: VX0003817_leg_L.fbx
  ? Output: VX0003817_leg_L_Resized_Y_2_00.fbx
    [Metadata] Node 'leg_L_loc.001': 'height' = 1.126 -> 2.252

=== Processing Complete ===
Total files processed: 1
Successful: 1
Failed: 0
```

## Multiple Properties on Same Node

If a node has multiple properties that match the axis:

```
Processing: VX0003817_leg_L.fbx
  ? Output: VX0003817_leg_L_Resized_Y_2_00.fbx
    [Metadata] Node 'leg_L_loc': 'height' = 1.126 -> 2.252
    [Metadata] Node 'leg_L_loc': 'y' = 0.500 -> 1.000
    [Metadata] Node 'leg_R_loc': 'height' = 1.126 -> 2.252
```

## When No Metadata Is Scaled

If a file has no custom properties to scale:

```
Processing: simple_mesh.fbx
  ? Output: simple_mesh_Resized_Y_2_00.fbx
```

No metadata lines appear - the log stays clean!

## Properties That Get Logged

### Y-Axis Scaling
- `height`, `Height`, `HEIGHT`
- `y`, `Y`

### X-Axis Scaling
- `width`, `Width`, `WIDTH`
- `x`, `X`

### Z-Axis Scaling
- `depth`, `Depth`, `DEPTH`
- `length`, `Length`, `LENGTH`
- `z`, `Z`

## Use Cases

### 1. Verify Metadata Is Scaling
**Before:** Wonder if metadata is being scaled  
**After:** See exactly which properties were modified!

```
? Output: model_Resized_Y_2_00.fbx
  [Metadata] Node 'bone_001': 'height' = 1.500 -> 3.000 ? Confirmed!
```

### 2. Debug Missing Properties
**No metadata logs?** ? Check:
- Property name matches expected (case-insensitive)
- Property is user-defined (not system property)
- Property is numeric (float/double)

### 3. Track All Changes
```
Processing: character_rig.fbx
  ? Output: character_rig_Resized_Y_2_00.fbx
    [Metadata] Node 'leg_L': 'height' = 1.126 -> 2.252
    [Metadata] Node 'leg_R': 'height' = 1.126 -> 2.252
    [Metadata] Node 'arm_L': 'height' = 0.850 -> 1.700
    [Metadata] Node 'arm_R': 'height' = 0.850 -> 1.700
    [Metadata] Node 'torso': 'height' = 1.200 -> 2.400
```

Clear overview of all metadata changes in one place!

### 4. Compare Before/After
Easily verify the math is correct:
```
[Metadata] Node 'marker': 'height' = 1.126 -> 2.252
Math check: 1.126 × 2.0 = 2.252 ?
```

## Benefits

? **Visible in main window** - No need to check debug output  
? **Immediate feedback** - See results as files are processed  
? **Easy to verify** - Confirm metadata is scaling correctly  
? **Clean format** - Indented under each file for clarity  
? **Scrollable log** - Review all changes after processing  
? **Copy-paste friendly** - Can copy log for documentation  

## Log Format Details

### Indentation
```
Processing: file.fbx                    ? File being processed
  ? Output: file_Resized_Y_2_00.fbx     ? Success indicator
    [Metadata] Node 'x': 'y' = 1 -> 2   ? Metadata (extra indent)
```

The metadata logs are indented with 4 spaces to show they're related to the file above.

### Node Name Format
- Uses FBX node names exactly as they appear in the file
- Typically: `bone_name`, `locator_001`, `attachment_point`, etc.

### Property Name Format
- Shows the exact property name from the FBX file
- Single quotes for clarity: `'height'`, `'width'`, `'y'`

### Value Format
- Shows 3 decimal places: `1.126`, `2.252`
- Arrow shows transformation: `old -> new`

## Example: Batch Processing

Processing multiple files with metadata:

```
Processing: VX0003816_leg_R.fbx
  ? Output: VX0003816_leg_R_Resized_Y_2_00.fbx
    [Metadata] Node 'leg_R_loc': 'height' = 1.126 -> 2.252

Processing: VX0003817_leg_L.fbx
  ? Output: VX0003817_leg_L_Resized_Y_2_00.fbx
    [Metadata] Node 'leg_L_loc': 'height' = 1.126 -> 2.252

Processing: VX0003818_arm_R.fbx
  ? Output: VX0003818_arm_R_Resized_Y_2_00.fbx
    [Metadata] Node 'arm_R_loc': 'height' = 0.850 -> 1.700

Processing: VX0003819_arm_L.fbx
  ? Output: VX0003819_arm_L_Resized_Y_2_00.fbx
    [Metadata] Node 'arm_L_loc': 'height' = 0.850 -> 1.700
```

Crystal clear what happened to each file!

## Troubleshooting

### "I don't see any metadata logs"

**Possible reasons:**
1. No custom properties in your FBX files
2. Properties don't match the axis being scaled
3. Properties aren't named 'height', 'width', etc.
4. Properties aren't numeric types

**How to check:**
- Open FBX in Blender/Maya
- Look for custom properties on bones/locators
- Verify property names
- Check property types (should be float/double)

### "Wrong values shown"

If values don't look right:
- Verify the input file property values
- Check the resize factor is correct
- Ensure you're looking at the right axis

## Comparison: Before vs. After

### Before (No Metadata Logs)
```
Processing: VX0003817_leg_L.fbx
  ? Output: VX0003817_leg_L_Resized_Y_2_00.fbx
```
? Did metadata scale? Unknown!

### After (With Metadata Logs)
```
Processing: VX0003817_leg_L.fbx
  ? Output: VX0003817_leg_L_Resized_Y_2_00.fbx
    [Metadata] Node 'leg_L_loc.001': 'height' = 1.126 -> 2.252
```
? Confirmed! Metadata scaled correctly!

## Summary

**What Changed:**
- Metadata scaling information now appears in UI log
- Shows node name, property name, old and new values
- Appears right after each successful file processing
- Clean, indented format for easy reading

**Benefits:**
- ? Immediate visual confirmation
- ? No need to check external tools
- ? Easy to verify correctness
- ? Great for documentation

**Build successful** - Restart the application to see metadata logs in the UI! ??
