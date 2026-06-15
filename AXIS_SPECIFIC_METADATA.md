# Axis-Specific Metadata Property Lists

## Major Update: Separate Property Lists Per Axis!

Now you can specify different metadata property names for each axis (X, Y, Z). This gives you precise control over which properties get scaled based on the axis you're processing.

## New UI Layout

**Three separate text fields:**

```
??????????????????????????????????????????????????????????????
? Metadata Property Names to Scale (comma-separated, per axis) ?
??????????????????????????????????????????????????????????????
? X-Axis Properties: ? width, x                              ?
??????????????????????????????????????????????????????????????
? Y-Axis Properties: ? height, y                             ?
??????????????????????????????????????????????????????????????
? Z-Axis Properties: ? depth, length, z                      ?
??????????????????????????????????????????????????????????????
Tip: Property names are case-insensitive. Only properties 
     for the selected axis will be scaled.
```

## How It Works

### Axis Selection Determines Which List is Used

When you select an axis and process files:
- **X-Axis selected** ? Uses X-Axis Properties list
- **Y-Axis selected** ? Uses Y-Axis Properties list
- **Z-Axis selected** ? Uses Z-Axis Properties list

### Example Workflow

**Setup:**
```
X-Axis Properties: width, x
Y-Axis Properties: height, y
Z-Axis Properties: depth, length, z

Selected Axis: Y
Resize Factor: 2.0
```

**Result:**
- Only properties named "height" or "y" will be scaled
- Properties named "width", "depth", "length", etc. will NOT be scaled
- Even if a node has all these properties, only Y-related ones are affected

## Why This Is Better

### Before (Single List)
```
All Properties: height, width, depth, length, x, y, z

Problem:
- Selecting Y-axis still considered ALL property names
- No axis-specific control
- Same properties for all axes
```

### After (Axis-Specific Lists)
```
X-Axis: width, x
Y-Axis: height, y
Z-Axis: depth, length, z

Benefits:
? Only relevant properties scaled per axis
? More precise control
? Logical separation by dimension
? Prevents accidental scaling
```

## Real-World Example

### Scenario: Character Model with Mixed Properties

**FBX File Contents:**
```
Node: character_root
  Custom Properties:
    - height = 1.8 (body height)
    - width = 0.5 (shoulder width)
    - depth = 0.3 (chest depth)
    - leg_length = 0.9
    - arm_length = 0.7
```

### Configuration
```
X-Axis Properties: width
Y-Axis Properties: height, leg_length, arm_length
Z-Axis Properties: depth
```

### Processing Y-Axis (Factor 2.0)

**Only Y-axis properties are scaled:**
```
Processing: character.fbx
  ? Output: character_Resized_Y_2_00.fbx
    [Metadata] Node 'character_root': 'height' = 1.800 -> 3.600
    [Metadata] Node 'character_root': 'leg_length' = 0.900 -> 1.800
    [Metadata] Node 'character_root': 'arm_length' = 0.700 -> 1.400
```

**Not scaled (not in Y-axis list):**
- `width` = 0.5 (unchanged - that's for X-axis)
- `depth` = 0.3 (unchanged - that's for Z-axis)

## Default Values

**X-Axis Properties:** `width, x`
- Common horizontal measurements
- Typically shoulder width, body width, etc.

**Y-Axis Properties:** `height, y`
- Common vertical measurements  
- Typically body height, bone lengths, etc.

**Z-Axis Properties:** `depth, length, z`
- Common depth/front-back measurements
- Typically chest depth, model length, etc.

## Use Cases

### Use Case 1: Accurate Body Proportions

**Goal:** Scale a character taller without making them wider

**Setup:**
```
X-Axis: width, shoulder_width
Y-Axis: height, leg_length, arm_length, torso_height
Z-Axis: depth

Process: Y-axis × 1.2
```

**Result:**
- Character is 20% taller ?
- Legs, arms, torso all scale proportionally ?
- Width and depth remain unchanged ?

### Use Case 2: Architectural Models

**Goal:** Make a building wider but not taller

**Setup:**
```
X-Axis: width, wall_width, floor_width
Y-Axis: height, floor_height, ceiling_height
Z-Axis: depth, room_depth

Process: X-axis × 2.0
```

**Result:**
- Building is twice as wide ?
- Height remains original ?
- Depth remains original ?

### Use Case 3: Weapon Scaling

**Goal:** Make a sword longer without changing grip size

**Setup:**
```
X-Axis: guard_width
Y-Axis: (empty - not used)
Z-Axis: blade_length, handle_length

Process: Z-axis × 1.5
```

**Result:**
- Blade and handle 50% longer ?
- Guard width unchanged ?
- Grip proportions preserved ?

## Advanced Configuration

### Complex Property Sets

**Character Rig:**
```
X-Axis Properties:
  shoulder_width, hip_width, hand_width

Y-Axis Properties:
  body_height, leg_length, arm_length, neck_height, 
  spine_length, torso_height

Z-Axis Properties:
  chest_depth, belly_depth, back_thickness
```

### Simple Property Sets

**Basic Markers:**
```
X-Axis: x
Y-Axis: y
Z-Axis: z
```

### Mixed Conventions

**Supporting Multiple File Types:**
```
X-Axis: width, w, x, horizontal_size
Y-Axis: height, h, y, vertical_size, altitude
Z-Axis: depth, d, z, length, front_back
```

This handles files with different naming conventions.

## Processing Examples

### Example 1: Y-Axis Scaling

**Configuration:**
```
Y-Axis Properties: height, y
Selected Axis: Y
Factor: 2.0
```

**Log Output:**
```
=== Starting FBX Processing ===
Resize Axis: Y
Resize Factor: 2.000000

Processing: model.fbx
  ? Output: model_Resized_Y_2_00.fbx
    [Metadata] Node 'bone1': 'height' = 1.126 -> 2.252
    [Metadata] Node 'bone2': 'y' = 0.850 -> 1.700
```

### Example 2: X-Axis Scaling

**Configuration:**
```
X-Axis Properties: width, x
Selected Axis: X
Factor: 1.5
```

**Log Output:**
```
=== Starting FBX Processing ===
Resize Axis: X
Resize Factor: 1.500000

Processing: model.fbx
  ? Output: model_Resized_X_1_50.fbx
    [Metadata] Node 'bone1': 'width' = 0.500 -> 0.750
    [Metadata] Node 'bone2': 'x' = 1.000 -> 1.500
```

### Example 3: Z-Axis Scaling

**Configuration:**
```
Z-Axis Properties: depth, length
Selected Axis: Z
Factor: 0.8
```

**Log Output:**
```
=== Starting FBX Processing ===
Resize Axis: Z
Resize Factor: 0.800000

Processing: model.fbx
  ? Output: model_Resized_Z_0_80.fbx
    [Metadata] Node 'bone1': 'depth' = 2.000 -> 1.600
    [Metadata] Node 'bone2': 'length' = 3.500 -> 2.800
```

## Benefits

? **Precision** - Scale only relevant properties per axis  
? **Flexibility** - Different properties for each dimension  
? **Clarity** - Obvious which properties belong to which axis  
? **Control** - Prevent unintended property scaling  
? **Logic** - Matches physical dimensions naturally  

## Workflow

### Step 1: Configure Property Lists
Set up property names for each axis based on your FBX files:
```
X-Axis: width, x
Y-Axis: height, y
Z-Axis: depth, length, z
```

### Step 2: Select Axis
Choose which axis to scale (X, Y, or Z)

### Step 3: Process
The application automatically uses the property list for the selected axis

### Step 4: Verify
Check the log to confirm which properties were scaled

## Tips

### Tip 1: Keep Lists Focused
Only include properties that make sense for each axis:
- X-Axis ? horizontal/width properties
- Y-Axis ? vertical/height properties  
- Z-Axis ? depth/length properties

### Tip 2: Use Descriptive Names
If your FBX uses custom names, add them:
```
Y-Axis: height, leg_height, arm_height, body_height
```

### Tip 3: Empty Lists Are OK
If an axis doesn't need metadata scaling:
```
Z-Axis: (leave empty)
```

### Tip 4: Case Doesn't Matter
Just type once:
- `height` matches `Height`, `HEIGHT`

## Comparison

### Old Behavior (Single List)
```
Properties: height, width, depth

Processing Y-axis:
- Considered: ALL properties (height, width, depth)
- Problem: No axis distinction
```

### New Behavior (Axis-Specific)
```
X-Axis: width
Y-Axis: height  
Z-Axis: depth

Processing Y-axis:
- Considered: ONLY height
- Result: Precise control ?
```

## Summary

**What Changed:**
- One metadata field ? Three axis-specific fields
- Single property list ? Separate lists for X, Y, Z
- All axes use same list ? Each axis uses its own list

**Benefits:**
- More precise metadata scaling
- Logical separation by dimension
- Prevents accidental scaling
- Better control over which properties are affected

**Default Values:**
- X-Axis: `width, x`
- Y-Axis: `height, y`
- Z-Axis: `depth, length, z`

**Build successful** - Restart and configure axis-specific property lists! ??
