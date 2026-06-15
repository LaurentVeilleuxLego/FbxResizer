# Final Behavior: Skip as Input, Overwrite as Output

## Perfect Balance Achieved ?

### What Happens Now

**Input Files (What Gets Processed):**
- ? `VX0003817_leg_L.fbx` ? **Processed**
- ? `VX0003817_leg_L_Resized_Y_2_00.fbx` ? **Skipped** (not used as input)

**Output Files (What Gets Created):**
- ? `VX0003817_leg_L_Resized_Y_2_00.fbx` ? **Overwritten** if exists

### Result

**No more cascading file names!**
- ? Won't create: `VX0003816_leg_R_Resized_Y_2_00_Resized_Y_2_00.fbx`
- ? Will create: `VX0003816_leg_R_Resized_Y_2_00.fbx` (overwritten if exists)

## Behavior Summary

### Skip Logic (Input Side)
```cpp
// Skip files that already have "_Resized_" in their name
if (filename.find(L"_Resized_") != std::wstring::npos)
{
    skippedCount++;
    continue;  // Don't process as input
}
```

**Purpose:** Prevents double-scaling and cascading file names

### Overwrite Logic (Output Side)
```cpp
// Output files are always created/overwritten
FbxProcessor::ProcessFbxFile(inputPath, outputPath, ...);
```

**Purpose:** Allows regenerating results without manual deletion

## Example Scenario

### First Run
```
Folder contents:
  - VX0003817_leg_L.fbx

Processing:
  ? Processing: VX0003817_leg_L.fbx
    ? Output: VX0003817_leg_L_Resized_Y_2_00.fbx

Result:
  - VX0003817_leg_L.fbx (original)
  - VX0003817_leg_L_Resized_Y_2_00.fbx (created)
```

### Second Run (Same Folder)
```
Folder contents:
  - VX0003817_leg_L.fbx
  - VX0003817_leg_L_Resized_Y_2_00.fbx

Processing:
  Overwriting: VX0003817_leg_L_Resized_Y_2_00.fbx
  ? Processing: VX0003817_leg_L.fbx
    ? Output: VX0003817_leg_L_Resized_Y_2_00.fbx (overwritten)
  (Skipped 1 file silently)

Result:
  - VX0003817_leg_L.fbx (original)
  - VX0003817_leg_L_Resized_Y_2_00.fbx (refreshed)
```

## Log Output

### Clean and Clear
```
Overwriting: VX0003817_leg_L_Resized_Y_2_00.fbx
Processing: VX0003817_leg_L.fbx
  ? Output: VX0003817_leg_L_Resized_Y_2_00.fbx

Processing: VX0003820_hand_R.fbx
  ? Output: VX0003820_hand_R_Resized_Y_2_00.fbx

=== Processing Complete ===
Total files processed: 8
Successful: 8
Failed: 0
Skipped (already resized): 8
```

### Note
- No individual "Skipping" messages cluttering the log
- Only shows what's being processed
- Summary shows skipped count at the end

## Benefits

? **No cascading names** - Never creates `_Resized_Y_2_00_Resized_Y_2_00`  
? **Safe to re-run** - Original files stay intact  
? **Automatic overwrite** - Results always fresh  
? **Clean logs** - Shows only active processing  
? **Smart counting** - Summary includes skipped files  

## Use Cases

### Iterative Development
1. Run with Y-axis 2.0
2. Check results in your 3D software
3. Adjust and re-run
4. Output files automatically updated

### Batch Processing
1. Copy many original FBX files to a folder
2. Process them all at once
3. Resized versions created alongside originals
4. Re-run anytime to refresh all outputs

### Mixed Folders
Works perfectly with folders containing:
- Original files (processed)
- Resized files (skipped as input, overwritten as output)
- Other files (ignored)

## What You'll Never See Again

? `VX0003816_leg_R_Resized_Y_2_00_Resized_Y_2_00.fbx`  
? `VX0003816_leg_R_Resized_Y_2_00_Resized_Y_2_00_Resized_Y_2_00.fbx`  
? Individual "Skipping" messages for every resized file  

## What You Will See

? `VX0003816_leg_R.fbx` (original)  
? `VX0003816_leg_R_Resized_Y_2_00.fbx` (clean, single-level resize)  
? "Overwriting" messages when updating existing files  
? Clean processing log  

**This is the perfect balance!** ??

**Restart the application** to use this behavior!
