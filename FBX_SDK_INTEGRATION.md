# FBX SDK Integration Guide

This document provides detailed instructions for integrating the Autodesk FBX SDK into the FBX File Resizer application.

## Step 1: Download FBX SDK

1. Visit the Autodesk Developer Network: https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-3-4
2. Download the FBX SDK for Windows (VS 2019 or later)
3. Run the installer and note the installation path (default: `C:\Program Files\Autodesk\FBX\FBX SDK\2020.3.4\`)

## Step 2: Update CMakeLists.txt

Replace the current CMakeLists.txt content with:

```cmake
# CMakeList.txt : CMake project for FbxResizer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("FbxResizer")

# FBX SDK Configuration
set(FBX_SDK_ROOT "C:/Program Files/Autodesk/FBX/FBX SDK/2020.3.4" CACHE PATH "FBX SDK root directory")
set(FBX_SDK_VERSION "2020.3.4")
set(FBX_SDK_INCLUDE_DIR "${FBX_SDK_ROOT}/include")

# Determine the correct library directory based on Visual Studio version and configuration
if(MSVC_VERSION GREATER_EQUAL 1930) # VS 2022
    set(FBX_SDK_LIB_DIR "${FBX_SDK_ROOT}/lib/vs2022/x64")
elseif(MSVC_VERSION GREATER_EQUAL 1920) # VS 2019
    set(FBX_SDK_LIB_DIR "${FBX_SDK_ROOT}/lib/vs2019/x64")
else()
    set(FBX_SDK_LIB_DIR "${FBX_SDK_ROOT}/lib/vs2017/x64")
endif()

# Add source to this project's executable.
add_executable (FbxResizer WIN32 
    "FbxResizer.cpp" 
    "FbxResizer.h" 
    "FbxProcessor.cpp"
    "FbxProcessor.h"
    "resource.h"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET FbxResizer PROPERTY CXX_STANDARD 20)
endif()

# Include FBX SDK headers
target_include_directories(FbxResizer PRIVATE ${FBX_SDK_INCLUDE_DIR})

# Link Windows libraries
target_link_libraries(FbxResizer PRIVATE comctl32)

# Link FBX SDK library (use different library for Debug vs Release)
target_link_libraries(FbxResizer PRIVATE 
    debug "${FBX_SDK_LIB_DIR}/debug/libfbxsdk-md.lib"
    optimized "${FBX_SDK_LIB_DIR}/release/libfbxsdk-md.lib"
)

# Copy FBX SDK DLL to output directory
add_custom_command(TARGET FbxResizer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<$<CONFIG:Debug>:${FBX_SDK_LIB_DIR}/debug/libfbxsdk.dll>$<$<CONFIG:Release>:${FBX_SDK_LIB_DIR}/release/libfbxsdk.dll>"
    $<TARGET_FILE_DIR:FbxResizer>
    COMMENT "Copying FBX SDK DLL to output directory"
)

# Add compiler definitions
target_compile_definitions(FbxResizer PRIVATE 
    FBXSDK_SHARED
)

# TODO: Add tests and install targets if needed.
```

## Step 3: Enable FBX SDK Code in FbxProcessor.cpp

At the top of `FbxProcessor.cpp`, uncomment the FBX SDK include:

```cpp
#include <fbxsdk.h>
```

Then uncomment and enable all the FBX SDK implementation code in the file.

## Step 4: Complete Implementation Template

Here's the complete implementation for `FbxProcessor.cpp` with FBX SDK integrated:

```cpp
#include "FbxProcessor.h"
#include <sstream>
#include <filesystem>
#include <fbxsdk.h>

namespace fs = std::filesystem;

namespace FbxProcessor
{
    // Global FBX SDK objects
    FbxManager* g_fbxManager = nullptr;
    FbxIOSettings* g_fbxIoSettings = nullptr;
    std::wstring g_lastError;

    // Forward declarations
    void ProcessNode(FbxNode* node, ResizeAxis axis, double scaleFactor);
    void ScaleMesh(FbxMesh* mesh, ResizeAxis axis, double scaleFactor);
    void ScaleSkeleton(FbxNode* node, ResizeAxis axis, double scaleFactor);

    bool InitializeFbxSdk(const std::wstring& sdkPath)
    {
        try
        {
            // Create the FBX Manager
            g_fbxManager = FbxManager::Create();
            if (!g_fbxManager)
            {
                g_lastError = L"Failed to create FBX Manager";
                return false;
            }

            // Create an IOSettings object
            g_fbxIoSettings = FbxIOSettings::Create(g_fbxManager, IOSROOT);
            g_fbxManager->SetIOSettings(g_fbxIoSettings);

            // Load plugins from the SDK path if provided
            // FbxString lPath = FbxGetApplicationDirectory();
            // g_fbxManager->LoadPluginsDirectory(lPath.Buffer());

            g_lastError.clear();
            return true;
        }
        catch (...)
        {
            g_lastError = L"Exception during FBX SDK initialization";
            return false;
        }
    }

    void CleanupFbxSdk()
    {
        if (g_fbxManager)
        {
            g_fbxManager->Destroy();
            g_fbxManager = nullptr;
            g_fbxIoSettings = nullptr;
        }
    }

    ProcessResult ProcessFbxFile(
        const std::wstring& inputPath,
        const std::wstring& outputPath,
        ResizeAxis axis,
        double scaleFactor)
    {
        ProcessResult result;
        result.inputFile = inputPath;
        result.outputFile = outputPath;
        result.success = false;

        // Check if input file exists
        if (!fs::exists(inputPath))
        {
            result.message = L"Input file does not exist";
            return result;
        }

        if (!g_fbxManager)
        {
            result.message = L"FBX Manager not initialized";
            return result;
        }

        FbxScene* scene = nullptr;
        FbxImporter* importer = nullptr;
        FbxExporter* exporter = nullptr;

        try
        {
            // Create a scene
            scene = FbxScene::Create(g_fbxManager, "TempScene");
            if (!scene)
            {
                result.message = L"Failed to create FBX scene";
                return result;
            }

            // Create an importer
            importer = FbxImporter::Create(g_fbxManager, "");

            // Convert wstring to string for FBX SDK
            std::string inputPathStr(inputPath.begin(), inputPath.end());

            // Initialize the importer
            if (!importer->Initialize(inputPathStr.c_str(), -1, g_fbxManager->GetIOSettings()))
            {
                FbxString error = importer->GetStatus().GetErrorString();
                result.message = L"Failed to initialize importer: " + 
                    std::wstring(error.Buffer(), error.Buffer() + error.GetLen());

                importer->Destroy();
                scene->Destroy();
                return result;
            }

            // Import the scene
            if (!importer->Import(scene))
            {
                FbxString error = importer->GetStatus().GetErrorString();
                result.message = L"Failed to import scene: " + 
                    std::wstring(error.Buffer(), error.Buffer() + error.GetLen());

                importer->Destroy();
                scene->Destroy();
                return result;
            }

            // Destroy the importer
            importer->Destroy();
            importer = nullptr;

            // Process the scene - scale along specified axis
            FbxNode* rootNode = scene->GetRootNode();
            if (rootNode)
            {
                // Recursively process all nodes
                ProcessNode(rootNode, axis, scaleFactor);
            }

            // Create an exporter
            exporter = FbxExporter::Create(g_fbxManager, "");
            std::string outputPathStr(outputPath.begin(), outputPath.end());

            // Initialize the exporter
            if (!exporter->Initialize(outputPathStr.c_str(), -1, g_fbxManager->GetIOSettings()))
            {
                FbxString error = exporter->GetStatus().GetErrorString();
                result.message = L"Failed to initialize exporter: " + 
                    std::wstring(error.Buffer(), error.Buffer() + error.GetLen());

                exporter->Destroy();
                scene->Destroy();
                return result;
            }

            // Export the scene
            if (!exporter->Export(scene))
            {
                FbxString error = exporter->GetStatus().GetErrorString();
                result.message = L"Failed to export scene: " + 
                    std::wstring(error.Buffer(), error.Buffer() + error.GetLen());

                exporter->Destroy();
                scene->Destroy();
                return result;
            }

            // Destroy the exporter
            exporter->Destroy();
            exporter = nullptr;

            // Destroy the scene
            scene->Destroy();
            scene = nullptr;

            result.success = true;
            result.message = L"Successfully processed";
        }
        catch (const std::exception& e)
        {
            std::string error = e.what();
            result.message = L"Exception: " + std::wstring(error.begin(), error.end());
            result.success = false;

            if (importer) importer->Destroy();
            if (exporter) exporter->Destroy();
            if (scene) scene->Destroy();
        }

        return result;
    }

    void ProcessNode(FbxNode* node, ResizeAxis axis, double scaleFactor)
    {
        if (!node) return;

        // Process mesh
        FbxMesh* mesh = node->GetMesh();
        if (mesh)
        {
            ScaleMesh(mesh, axis, scaleFactor);
        }

        // Process skeleton
        FbxSkeleton* skeleton = node->GetSkeleton();
        if (skeleton)
        {
            ScaleSkeleton(node, axis, scaleFactor);
        }

        // Recursively process children
        for (int i = 0; i < node->GetChildCount(); ++i)
        {
            ProcessNode(node->GetChild(i), axis, scaleFactor);
        }
    }

    void ScaleMesh(FbxMesh* mesh, ResizeAxis axis, double scaleFactor)
    {
        if (!mesh) return;

        int controlPointCount = mesh->GetControlPointsCount();
        FbxVector4* controlPoints = mesh->GetControlPoints();

        for (int i = 0; i < controlPointCount; ++i)
        {
            switch (axis)
            {
            case ResizeAxis::X:
                controlPoints[i][0] *= scaleFactor;
                break;
            case ResizeAxis::Y:
                controlPoints[i][1] *= scaleFactor;
                break;
            case ResizeAxis::Z:
                controlPoints[i][2] *= scaleFactor;
                break;
            }
        }
    }

    void ScaleSkeleton(FbxNode* node, ResizeAxis axis, double scaleFactor)
    {
        if (!node) return;

        // Scale the node's local translation
        FbxDouble3 translation = node->LclTranslation.Get();

        switch (axis)
        {
        case ResizeAxis::X:
            translation[0] *= scaleFactor;
            break;
        case ResizeAxis::Y:
            translation[1] *= scaleFactor;
            break;
        case ResizeAxis::Z:
            translation[2] *= scaleFactor;
            break;
        }

        node->LclTranslation.Set(translation);
    }

    std::wstring GetLastError()
    {
        return g_lastError;
    }
}
```

## Step 5: Rebuild the Project

After making these changes:

1. Clean the build directory:
   ```bash
   cd build
   del /s /q *
   ```

2. Reconfigure and build:
   ```bash
   cmake .. -G "Visual Studio 17 2022" -A x64
   cmake --build . --config Release
   ```

## Step 6: Test the Application

1. Launch the application
2. Set the FBX SDK DLL folder path
3. Select a folder with FBX files
4. Choose an axis and resize factor
5. Click "Process FBX Files"
6. Check the log for results

## Troubleshooting

### Common Issues

1. **"Cannot open include file: 'fbxsdk.h'"**
   - Solution: Verify FBX_SDK_ROOT path in CMakeLists.txt

2. **"Unresolved external symbol"**
   - Solution: Check that the correct FBX SDK library (libfbxsdk-md.lib) is linked
   - Make sure you're using the multi-threaded DLL runtime (/MD)

3. **"libfbxsdk.dll not found" at runtime**
   - Solution: Ensure the DLL is copied to the output directory
   - Check the POST_BUILD command in CMakeLists.txt

4. **Import/Export fails**
   - Solution: Make sure FBX SDK plugins are loaded
   - Check file paths are valid
   - Verify FBX file format is supported

### Debug Tips

- Use the processing log to see detailed error messages
- Check FbxStatus error strings for FBX SDK specific errors
- Enable FBX SDK verbose logging for more details
- Test with simple FBX files first

## Performance Considerations

- Large FBX files may take time to process
- Consider adding progress indicators for long operations
- Batch processing is already optimized for multiple files

## Additional Resources

- FBX SDK Documentation: https://help.autodesk.com/view/FBX/2020/ENU/
- FBX SDK Samples: Located in `<FBX_SDK_ROOT>/samples/`
- API Reference: https://help.autodesk.com/view/FBX/2020/ENU/?guid=FBX_Developer_Help_html
