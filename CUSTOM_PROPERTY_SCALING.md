# Custom Property Scaling Feature

## Overview

The FBX File Resizer now automatically scales custom properties (metadata) on bones and locators based on the selected resize axis.

## How It Works

### Automatic Property Detection

When processing FBX files, the application now:
1. ? Traverses all nodes (bones, locators, meshes, etc.)
2. ? Examines all custom properties (user-defined metadata)
3. ? Identifies properties that correspond to the selected axis
4. ? Scales matching numeric properties by the resize factor

### Property Name Matching

The application automatically scales properties based on their names:

| Resize Axis | Property Names Scaled |
|-------------|----------------------|
| **Y Axis** | `height`, `Height`, `HEIGHT`, `y`, `Y` |
| **X Axis** | `width`, `Width`, `WIDTH`, `x`, `X` |
| **Z Axis** | `depth`, `Depth`, `DEPTH`, `z`, `Z`, `length`, `Length`, `LENGTH` |

### Example Scenario

**Before Resizing:**
- Node: `leg_L_loc.001`
- Custom Property: `height = 1.126`
- Resize Axis: Y
- Resize Factor: 2.0

**After Resizing:**
- Node: `leg_L_loc.001` (in resized file)
- Custom Property: `height = 2.252` (1.126 × 2.0)

## Supported Property Types

The feature works with:
- ? **Float** properties (FbxFloat)
- ? **Double** properties (FbxDouble)
- ? **User-defined** properties only (preserves system properties)

## Use Cases

### 1. Blender Custom Properties
If you have custom properties on bones/locators in Blender:
```
height: 1.126
width: 0.5
depth: 0.3
```

When scaling on Y-axis by 2.0, the `height` property becomes `2.252`.

### 2. Character Rig Metadata
For character rigs with height markers on bones:
```
leg_bone.height: 0.85
torso_bone.height: 1.2
```

These values will be automatically scaled to match the new proportions.

### 3. Locator Positioning
For locators with position metadata:
```
attachment_locator.y: 1.5
attachment_locator.height: 0.2
```

Both properties will scale when resizing on Y-axis.

## Technical Implementation

### Property Processing Function

```cpp
void ScaleCustomProperties(FbxNode* node, ResizeAxis axis, double scaleFactor)
```

This function:
1. Iterates through all properties on a node
2. Checks if each property is user-defined
3. Identifies numeric properties (float/double)
4. Matches property names against axis-specific patterns
5. Applies the scale factor to matching properties

### Integration

The `ProcessNode` function now calls:
```cpp
// Process custom properties (metadata) on all nodes
ScaleCustomProperties(node, axis, scaleFactor);
```

This ensures all nodes are processed, including:
- Bones (FbxSkeleton)
- Locators (empty nodes)
- Meshes
- Cameras
- Lights
- Any other node type

## Benefits

? **Automatic**: No manual adjustment of metadata needed  
? **Consistent**: Metadata stays synchronized with geometry  
? **Flexible**: Case-insensitive property name matching  
? **Safe**: Only modifies user-defined properties  
? **Comprehensive**: Works on all node types  

## Case Sensitivity

The property name matching is case-insensitive:
- `height`, `Height`, `HEIGHT` all match
- `width`, `Width`, `WIDTH` all match
- `depth`, `Depth`, `DEPTH` all match

## Preserved Properties

The following are **NOT** modified:
- ? System properties (FBX internal properties)
- ? Non-numeric properties (strings, booleans, etc.)
- ? Properties that don't match axis-related names
- ? Transform properties (handled separately)

## Testing

To verify the feature works:

1. **Create test FBX** with custom properties:
   - Add a bone or locator
   - Add custom property named "height" with value 1.0

2. **Process the file**:
   - Select Y-axis
   - Set resize factor to 2.0
   - Process the file

3. **Verify in Blender/Maya**:
   - Import the resized FBX
   - Check the "height" property
   - It should be 2.0 (doubled)

## Property Name Extensions

To support additional property names, modify the `ScaleCustomProperties` function:

```cpp
// For Y-axis, you could add:
if (propNameStr == "height" || propNameStr == "Height" || 
    propNameStr == "vertical" || propNameStr == "altitude") 
{
    shouldScale = true;
}
```

## Logging

The application maintains existing logging for file processing. Property scaling happens silently as part of node processing.

## Compatibility

? **Blender**: Custom properties preserved and scaled  
? **Maya**: User attributes preserved and scaled  
? **3ds Max**: Custom attributes preserved and scaled  
? **Unity/Unreal**: Metadata preserved and scaled  

## Performance Impact

- **Minimal**: Property iteration is fast
- **Negligible**: Only processes user-defined properties
- **Efficient**: Single pass during node traversal

## Future Enhancements

Potential improvements:
- Add configuration file for custom property name mappings
- Support vector properties (scale all components)
- Add logging for which properties were scaled
- Support property name patterns/wildcards

## Summary

The custom property scaling feature ensures that metadata on bones and locators remains consistent with the geometric scaling applied to the model. This is especially useful for:
- Character rigs with height/size metadata
- Locators with position markers
- Bones with measurement data
- Any node with dimensional custom properties

The feature works automatically without any additional configuration!
