# User-Configurable Metadata Property Names

## What's New

You can now specify which custom property names should be scaled through the UI! No more hardcoded property names.

## New UI Field

**"Metadata Property Names to Scale (comma-separated)"**

Default value: `height, width, depth, length, x, y, z`

## How It Works

### Simple Configuration
Just enter the property names you want to scale, separated by commas:

```
height, width, depth
```

Or:

```
Height, MyCustomProp, marker_y, bone_size
```

### Case-Insensitive Matching
The property names are matched case-insensitively, so:
- `height` matches `height`, `Height`, `HEIGHT`
- `width` matches `width`, `Width`, `WIDTH`
- `MyProp` matches `myprop`, `MYPROP`, `MyProp`

### Applies to All Axes
The same list of property names is used for all axes (X, Y, Z). If a property exists in your FBX file and matches a name in the list, it will be scaled.

## Examples

### Example 1: Height Only
```
Input: height
```

**Result:** Only properties named "height" (any case) will be scaled

**Use case:** You only want to scale vertical measurements

### Example 2: Multiple Standard Names
```
Input: height, width, depth, length
```

**Result:** All dimensional properties are scaled

**Use case:** Standard dimensional metadata

### Example 3: Custom Property Names
```
Input: leg_height, arm_length, custom_size, marker_y
```

**Result:** Your specific custom properties are scaled

**Use case:** You have custom property naming conventions

### Example 4: Simple Axis Names
```
Input: x, y, z
```

**Result:** Only properties named x, y, or z are scaled

**Use case:** Simplified axis-based properties

### Example 5: Mixed Names
```
Input: height, Height, leg_size, arm_length, y, Y
```

**Result:** All listed names (case-insensitive)

**Note:** You don't need to list multiple cases - `height` will match `Height` automatically

## UI Example

```
??????????????????????????????????????????????????????????????
? Metadata Property Names to Scale (comma-separated)        ?
??????????????????????????????????????????????????????????????
? height, width, depth, length, x, y, z                     ?
??????????????????????????????????????????????????????????????
Tip: Property names are case-insensitive. For Y-axis, 
     'height' and 'y' will be scaled.
```

## Processing Example

### Setup
```
Metadata Names: height, custom_size
Resize Axis: Y
Resize Factor: 2.0
```

### Input FBX
```
Node: leg_L_loc
  Custom Properties:
    - height = 1.126
    - custom_size = 2.5
    - width = 0.8 (won't be scaled - not in list)
```

### Output Log
```
Processing: leg_model.fbx
  ? Output: leg_model_Resized_Y_2_00.fbx
    [Metadata] Node 'leg_L_loc': 'height' = 1.126 -> 2.252
    [Metadata] Node 'leg_L_loc': 'custom_size' = 2.500 -> 5.000
```

**Note:** `width` was not scaled because it wasn't in the list

## Whitespace Handling

The code automatically trims spaces, so these are all equivalent:

```
height,width,depth
height, width, depth
height , width , depth
  height  ,  width  ,  depth  
```

## Empty List Behavior

If you leave the field empty or clear it:
- **Result:** No metadata properties will be scaled
- **Effect:** Only mesh and skeleton are scaled, custom properties are untouched

## Default Configuration

The default list covers common property names:

```
height, width, depth, length, x, y, z
```

This works for most common use cases:
- Blender custom properties
- Maya user attributes  
- Unity/Unreal metadata
- Standard dimensional properties

## Workflow: Finding Property Names

### Step 1: Check Your FBX in Blender/Maya
1. Import your FBX file
2. Select a bone or locator
3. Look for custom properties
4. Note the exact property names

### Step 2: Add Names to UI
Enter the property names you found, separated by commas

### Step 3: Process and Verify
Check the log to see which properties were scaled:

```
[Metadata] Node 'bone_001': 'height' = 1.0 -> 2.0  ? Found!
```

## Advanced Usage

### Scenario 1: Different Models, Same Convention
```
Models: character_A.fbx, character_B.fbx, character_C.fbx
All use properties: "height", "width"

Setup: height, width
Result: Works for all models
```

### Scenario 2: Mixed Conventions
```
Model A uses: height, width
Model B uses: h, w
Model C uses: vertical_size, horizontal_size

Setup: height, width, h, w, vertical_size, horizontal_size
Result: Works for all models
```

### Scenario 3: Selective Scaling
```
Properties in FBX: height, width, depth, internal_id, name

Want to scale: height, width, depth
Don't want to scale: internal_id, name

Setup: height, width, depth
Result: Only dimensional properties scaled, IDs preserved
```

## Benefits

? **Flexibility** - Use any property names you want  
? **No recompile** - Change property names without rebuilding  
? **Project-specific** - Adapt to your naming conventions  
? **Transparent** - See exactly what gets scaled in the log  
? **Safe** - Only specified properties are affected  

## Comparison: Before vs After

### Before (Hardcoded)
```cpp
// Code had hardcoded checks:
if (propName == "height" || propName == "Height" || ...)
```

**Problems:**
- ? Fixed property names
- ? Need code changes to add names
- ? Recompile required
- ? Not flexible for custom conventions

### After (User-Configurable)
```
UI Field: height, width, depth, custom_prop
```

**Benefits:**
- ? Any property names
- ? Change anytime through UI
- ? No recompile
- ? Adapts to your project

## Tips

### Tip 1: Start Simple
Begin with: `height, width, depth`

Add more as needed

### Tip 2: Check the Log
The processing log shows which properties were found and scaled:

```
[Metadata] Node 'bone': 'height' = 1.0 -> 2.0  ? Found!
```

No log entry? The property doesn't exist or has a different name.

### Tip 3: Case Doesn't Matter
Just type the name once:
- `height` matches Height, HEIGHT, height
- Don't need: `height, Height, HEIGHT`

### Tip 4: Use Descriptive Names
Good: `leg_height, arm_length`  
Also good: `height, length`

Choose what makes sense for your project.

### Tip 5: Save Your Configuration
The field remembers your settings between runs within the same session.

## Troubleshooting

### "Property not scaling"

**Check:**
1. Is the property name in the UI field?
2. Is it spelled correctly?
3. Is it a custom (user-defined) property?
4. Is it a numeric type (float/double)?

**Solution:** Add the exact property name to the UI field

### "Too many properties scaling"

**Check:** Remove unwanted property names from the UI field

### "Properties scaling on wrong axis"

**Note:** All properties in the list are scaled regardless of axis. If you only want certain properties for certain axes, process files separately with different property lists.

## Summary

**What Changed:**
- Added UI field for property names
- Properties are no longer hardcoded
- User can specify any property names
- Case-insensitive matching
- Comma-separated list

**Default Value:**
`height, width, depth, length, x, y, z`

**How to Use:**
1. Open the application
2. Edit "Metadata Property Names" field
3. Enter your property names (comma-separated)
4. Process your files
5. Check the log to verify

**Build successful** - Restart and configure your property names! ???
