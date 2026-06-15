# Quick Start Guide

## Running the Application (Without FBX SDK)

The application is built and ready to run! It currently works in "demo mode" (file copy) until you integrate the FBX SDK.

### Launch the Application

From the build directory:
```bash
.\Release\FbxResizer.exe
```

Or from Visual Studio: Press F5 to debug or Ctrl+F5 to run without debugging.

### Using the Interface

1. **FBX SDK DLL Folder**: Enter or browse to your FBX SDK DLL location
   - Default placeholder: `C:\Program Files\Autodesk\FBX\FBX SDK\2020.3.4\lib\vs2021\x64\release`

2. **Select FBX Folder**: Choose the folder containing your .fbx files
   - Click "Browse..." to use a folder picker dialog

3. **Select Resize Axis**: Choose which axis to scale
   - ? X Axis
   - ? Y Axis (default)
   - ? Z Axis

4. **Resize Factor**: Enter the scale multiplier
   - Default: 2.00 (double size)
   - Examples: 0.5 (half size), 1.5 (150%), 3.0 (triple)

5. **Process FBX Files**: Click to start processing
   - The log will show progress and results

### Output Files

Processed files are saved in the same folder as the input files with names like:
```
OriginalName_Resized_Y_2_00.fbx
```

### Example Workflow

1. Place your FBX files in a folder, e.g., `C:\MyModels\`
2. Launch FbxResizer.exe
3. Browse to `C:\MyModels\` for the FBX Folder
4. Select Y Axis (for vertical scaling)
5. Enter 2.00 as the resize factor
6. Click "Process FBX Files"
7. Check the log for results
8. Find resized files in `C:\MyModels\` with "_Resized_Y_2_00" suffix

### Current Functionality

**Without FBX SDK Integration:**
- ? Full GUI is functional
- ? File browsing and validation
- ? Batch processing of multiple FBX files
- ? Proper output file naming
- ?? Files are copied (not actually resized)
- ?? No mesh or skeleton modification yet

**With FBX SDK Integration:**
- ? All above features
- ? Actual mesh vertex scaling
- ? Skeleton bone translation scaling
- ? Full FBX import/export
- ? Preserves all FBX data (materials, animations, etc.)

### Next Steps

To enable full FBX processing:
1. Download and install Autodesk FBX SDK
2. Follow the instructions in `FBX_SDK_INTEGRATION.md`
3. Rebuild the project
4. Test with your FBX files

### System Requirements

- Windows 10 or later
- Visual Studio 2019+ Runtime (VC++ Redistributable)
- FBX SDK 2020.3.4+ (for full functionality)

### Support

Check the logs for detailed error messages if anything goes wrong. The processing log shows:
- Which files are being processed
- Success/failure status for each file
- Detailed error messages
- Summary statistics

Enjoy using FBX File Resizer!
