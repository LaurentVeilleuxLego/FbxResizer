# FBX File Resizer

A Win32 GUI application for batch resizing FBX files along a specified axis (X, Y, or Z). The application scales both the mesh geometry and skeleton bones.

## Features

- **Batch Processing**: Process multiple FBX files in a folder at once
- **Axis Selection**: Choose to resize along X, Y, or Z axis
- **Configurable Scale Factor**: Specify any resize multiplier (e.g., 2.0 for double size, 0.5 for half size)
- **Skeleton Support**: Properly scales skeleton bones along with mesh geometry
- **Processing Log**: Real-time feedback on processing status
- **Output Naming**: Automatic output file naming in the format: `<original_name>_Resized_<AXIS>_<FACTOR>.fbx`
  - Example: `VX0003817_leg_L_Resized_Y_2_00.fbx`

## Building the Project

### Prerequisites

- Windows 10 or later
- Visual Studio 2019 or later (with C++ desktop development workload)
- CMake 3.8 or later
- Autodesk FBX SDK 2020.3.4 or later (for actual FBX processing)

### Build Steps

1. **Clone or download the project**

2. **Configure with CMake**:
   ```bash
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64
   ```

3. **Build the project**:
   ```bash
   cmake --build . --config Release
   ```

   Or open the generated solution file in Visual Studio and build from there.

## Integrating FBX SDK

The current implementation includes a placeholder for FBX SDK integration. To add full FBX processing capabilities:

### 1. Download and Install FBX SDK

Download the FBX SDK from Autodesk:
https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-3-4

Install it to a location like:
`C:\Program Files\Autodesk\FBX\FBX SDK\2020.3.4\`

### 2. Update CMakeLists.txt

Add the following after the project declaration:

```cmake
# FBX SDK Configuration
set(FBX_SDK_ROOT "C:/Program Files/Autodesk/FBX/FBX SDK/2020.3.4" CACHE PATH "FBX SDK root directory")
set(FBX_SDK_INCLUDE_DIR "${FBX_SDK_ROOT}/include")
set(FBX_SDK_LIB_DIR "${FBX_SDK_ROOT}/lib/vs2019/x64/release")

# Include FBX SDK headers
target_include_directories(FbxResizer PRIVATE ${FBX_SDK_INCLUDE_DIR})

# Link FBX SDK library
target_link_libraries(FbxResizer PRIVATE 
    comctl32
    "${FBX_SDK_LIB_DIR}/libfbxsdk-md.lib"
)

# Copy FBX SDK DLL to output directory
add_custom_command(TARGET FbxResizer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${FBX_SDK_LIB_DIR}/libfbxsdk.dll"
    $<TARGET_FILE_DIR:FbxResizer>
)
```

### 3. Uncomment FBX SDK Code

In `FbxProcessor.cpp`, uncomment:
- The `#include <fbxsdk.h>` line
- The FBX SDK initialization code
- The actual processing implementation
- The helper function `ProcessNode`

### 4. Rebuild the Project

After making these changes, rebuild the project. The application will now have full FBX processing capabilities.

## Usage

1. **Launch the application**: Run `FbxResizer.exe`

2. **Configure Settings**:
   - **FBX SDK DLL Folder**: Path to the FBX SDK's DLL directory (e.g., `C:\Program Files\Autodesk\FBX\FBX SDK\2020.3.4\lib\vs2021\x64\release`)
   - **Select FBX Folder**: Choose the folder containing your FBX files to process
   - **Select Resize Axis**: Choose X, Y, or Z axis
   - **Resize Factor**: Enter the scale multiplier (e.g., 2.00 for double size, 0.5 for half size)

3. **Process Files**: Click "Process FBX Files" button

4. **Review Results**: Check the processing log for details on each file processed

## Output File Naming

Output files follow this naming convention:
```
<OriginalName>_Resized_<Axis>_<Factor>.fbx
```

Examples:
- Input: `VX0003817_leg_L.fbx`, Axis: Y, Factor: 2.00
  - Output: `VX0003817_leg_L_Resized_Y_2_00.fbx`
- Input: `character_model.fbx`, Axis: X, Factor: 1.50
  - Output: `character_model_Resized_X_1_50.fbx`

## Architecture

### Files

- **FbxResizer.cpp/h**: Main Win32 GUI application
- **FbxProcessor.cpp/h**: FBX processing logic (SDK integration)
- **resource.h**: Resource definitions
- **CMakeLists.txt**: CMake build configuration

### Processing Logic

The application processes FBX files by:
1. Loading each FBX file using the FBX SDK
2. Traversing the scene graph to find all nodes
3. For mesh nodes: Scaling vertex positions along the selected axis
4. For skeleton nodes: Scaling bone translations along the selected axis
5. Exporting the modified scene to a new FBX file

## Known Limitations

- Currently, the application uses a placeholder implementation (file copy) until FBX SDK is integrated
- Animation data is preserved but not modified
- Only the selected axis is scaled; other axes remain unchanged

## Troubleshooting

### Build Errors

- Ensure Visual Studio C++ desktop development workload is installed
- Check that CMake version is 3.8 or later
- Verify Windows SDK is installed

### FBX SDK Integration Issues

- Make sure the FBX SDK path in CMakeLists.txt matches your installation
- Use the correct library variant (md for multi-threaded DLL runtime)
- Ensure the FBX SDK DLL is accessible at runtime

### Runtime Issues

- If the application fails to start, ensure all required DLLs are in the executable directory
- Check the processing log for detailed error messages
- Verify the FBX folder path exists and contains .fbx files

## License

This project is provided as-is for educational and commercial use.

## Support

For FBX SDK documentation and support, visit:
- https://help.autodesk.com/view/FBX/2020/ENU/

For issues with this application, check the processing log for detailed error information.
