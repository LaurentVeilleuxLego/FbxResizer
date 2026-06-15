# File Overwrite Behavior Update

## Changes Made

### Previous Behavior
- ? Files with "_Resized_" in their name were **skipped** entirely
- ? Previously generated resized files could not be regenerated

### New Behavior
- ? Files with "_Resized_" in their **source** name are still skipped (to avoid processing already resized files)
- ? **Output files are now overwritten** if they already exist
- ? You can now re-run the process with different settings and the output will be updated

## Example Scenario

### Input Files
```
VX0003820_hand_R.fbx
VX0003820_hand_R_Resized_Y_2_00.fbx (previously generated)
```

### First Run (Y-axis, 2.0)
```
? Processing: VX0003820_hand_R.fbx
? Output: VX0003820_hand_R_Resized_Y_2_00.fbx
? Skipping (already resized): VX0003820_hand_R_Resized_Y_2_00.fbx
```

### Second Run (Y-axis, 2.0) - Same Settings
```
Overwriting existing: VX0003820_hand_R_Resized_Y_2_00.fbx
? Processing: VX0003820_hand_R.fbx
? Output: VX0003820_hand_R_Resized_Y_2_00.fbx (overwritten)
? Skipping (already resized): VX0003820_hand_R_Resized_Y_2_00.fbx
```

### Third Run (Y-axis, 3.0) - Different Settings
```
Overwriting existing: VX0003820_hand_R_Resized_Y_2_00.fbx
? Processing: VX0003820_hand_R.fbx
? Output: VX0003820_hand_R_Resized_Y_3_00.fbx (new file)
? Skipping (already resized): VX0003820_hand_R_Resized_Y_2_00.fbx
```

## What Gets Overwritten

? **Will be overwritten:**
- Any file matching the output pattern: `OriginalName_Resized_AXIS_FACTOR.fbx`
- Example: `hand_R_Resized_Y_2_00.fbx` can be regenerated

? **Will NOT be processed:**
- Source files with "_Resized_" in their name
- Example: `hand_R_Resized_Y_2_00.fbx` won't be used as an input to create `hand_R_Resized_Y_2_00_Resized_Y_2_00.fbx`

## Use Cases

### 1. Re-run After Code Updates
When you update the processing logic (like adding custom property scaling), you can now:
1. Keep the same input files
2. Re-run the processor
3. The output files will be regenerated with the new logic

### 2. Experiment with Different Settings
You can now:
1. Process with Y-axis, 2.0 ? generates `file_Resized_Y_2_00.fbx`
2. Process with Y-axis, 1.5 ? generates `file_Resized_Y_1_50.fbx`
3. Re-process with Y-axis, 2.0 ? **overwrites** `file_Resized_Y_2_00.fbx`

### 3. Fix Mistakes
If you process with wrong settings:
1. Notice the output is wrong
2. Re-run with correct settings
3. Output file is overwritten with correct version

## Logging Changes

The log now shows:
```
Overwriting existing: VX0003820_hand_R_Resized_Y_2_00.fbx
Processing: VX0003820_hand_R.fbx
  ? Output: VX0003820_hand_R_Resized_Y_2_00.fbx
```

This clearly indicates when an existing file is being replaced.

## Safety Features

The application still prevents:
- ? Processing already-resized files as input (prevents cascading scaling)
- ? Creating files like `file_Resized_Y_2_00_Resized_Y_2_00.fbx`

But now allows:
- ? Overwriting output files with the same name
- ? Re-running the process multiple times
- ? Updating results after code changes

## Performance Note

Overwriting files is just as fast as creating new files. The FBX exporter automatically handles file replacement.

## Best Practices

### Recommended Workflow
1. Keep original source FBX files in one folder
2. Process them to generate `_Resized_` versions
3. Re-run whenever needed - outputs will be overwritten
4. Original files remain untouched

### Folder Organization
```
MyModels/
??? VX0003820_hand_R.fbx              (original - always kept)
??? VX0003820_hand_L.fbx              (original - always kept)
??? VX0003820_hand_R_Resized_Y_2_00.fbx  (generated - can be overwritten)
??? VX0003820_hand_L_Resized_Y_2_00.fbx  (generated - can be overwritten)
```

## Summary

**Before:**
- ?? Once generated, output files couldn't be regenerated without manual deletion

**After:**
- ?? Output files are automatically overwritten when processing the same source with the same settings
- ?? Clear logging shows when files are being overwritten
- ? More convenient for iterative development and testing

**Restart the application** to use the new behavior!
