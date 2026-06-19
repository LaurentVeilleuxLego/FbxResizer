#include "FbxProcessor.h"
#include <sstream>
#include <filesystem>
#include <windows.h>
#include <fbxsdk.h>

namespace fs = std::filesystem;

namespace FbxProcessor
{
    // Global FBX SDK objects
    FbxManager* g_fbxManager = nullptr;
    FbxIOSettings* g_fbxIoSettings = nullptr;
    std::wstring g_lastError;
    std::vector<std::wstring> g_metadataLogs;  // Collect metadata scaling logs
    std::vector<std::wstring> g_metadataPropertyNames;  // User-specified property names

    // Helper function to convert wstring to string
    std::string WStringToString(const std::wstring& wstr)
    {
        if (wstr.empty()) return std::string();
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
        return strTo;
    }

    // Helper function to convert string to wstring
    std::wstring StringToWString(const std::string& str)
    {
        if (str.empty()) return std::wstring();
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
        return wstrTo;
    }

    // Forward declarations
    void ProcessNode(FbxNode* node, ResizeAxis axis, double scaleFactor, double rotationDegrees);
    void ScaleMesh(FbxMesh* mesh, ResizeAxis axis, double scaleFactor);
    void ScaleSkeleton(FbxNode* node, ResizeAxis axis, double scaleFactor);
    void ScaleCustomProperties(FbxNode* node, ResizeAxis axis, double scaleFactor);
    void RotateNode(FbxNode* node, ResizeAxis axis, double rotationDegrees);
    void RotateHierarchy(FbxNode* node, ResizeAxis axis, double rotationDegrees);

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
        double scaleFactor,
        double rotationDegrees,
        const std::vector<std::wstring>& metadataPropertyNames)
    {
        ProcessResult result;
        result.inputFile = inputPath;
        result.outputFile = outputPath;
        result.success = false;

        // Clear previous metadata logs
        g_metadataLogs.clear();

        // Store property names for ScaleCustomProperties to use
        g_metadataPropertyNames = metadataPropertyNames;

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
            std::string inputPathStr = WStringToString(inputPath);

            // Initialize the importer
            if (!importer->Initialize(inputPathStr.c_str(), -1, g_fbxManager->GetIOSettings()))
            {
                FbxString error = importer->GetStatus().GetErrorString();
                result.message = L"Failed to initialize importer: " + StringToWString(error.Buffer());

                importer->Destroy();
                scene->Destroy();
                return result;
            }

            // Import the scene
            if (!importer->Import(scene))
            {
                FbxString error = importer->GetStatus().GetErrorString();
                result.message = L"Failed to import scene: " + StringToWString(error.Buffer());

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
                ProcessNode(rootNode, axis, scaleFactor, rotationDegrees);

                // Apply rotation to all direct children of root (actual model nodes)
                if (rotationDegrees != 0.0)
                {
                    RotateHierarchy(rootNode, axis, rotationDegrees);
                }
            }

            // Create an exporter
            exporter = FbxExporter::Create(g_fbxManager, "");
            std::string outputPathStr = WStringToString(outputPath);

            // Initialize the exporter
            if (!exporter->Initialize(outputPathStr.c_str(), -1, g_fbxManager->GetIOSettings()))
            {
                FbxString error = exporter->GetStatus().GetErrorString();
                result.message = L"Failed to initialize exporter: " + StringToWString(error.Buffer());

                exporter->Destroy();
                scene->Destroy();
                return result;
            }

            // Export the scene
            if (!exporter->Export(scene))
            {
                FbxString error = exporter->GetStatus().GetErrorString();
                result.message = L"Failed to export scene: " + StringToWString(error.Buffer());

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
            result.metadataLogs = g_metadataLogs;  // Copy collected metadata logs
        }
        catch (const std::exception& e)
        {
            result.message = L"Exception: " + StringToWString(e.what());
            result.success = false;

            if (importer) importer->Destroy();
            if (exporter) exporter->Destroy();
            if (scene) scene->Destroy();
        }

        return result;
    }

    void ProcessNode(FbxNode* node, ResizeAxis axis, double scaleFactor, double rotationDegrees)
    {
        if (!node) return;

        // Process mesh
        FbxMesh* mesh = node->GetMesh();
        if (mesh)
        {
            ScaleMesh(mesh, axis, scaleFactor);
        }

        // Scale the node's local translation (for all nodes, not just skeleton bones)
        // This ensures bones, locators, and other transform nodes are scaled
        ScaleSkeleton(node, axis, scaleFactor);

        // Process custom properties (metadata) on all nodes
        ScaleCustomProperties(node, axis, scaleFactor);

        // Recursively process children
        for (int i = 0; i < node->GetChildCount(); ++i)
        {
            ProcessNode(node->GetChild(i), axis, scaleFactor, rotationDegrees);
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

    void ScaleCustomProperties(FbxNode* node, ResizeAxis axis, double scaleFactor)
    {
        if (!node) return;

        // Get node name for logging
        const char* nodeName = node->GetName();
        std::string nodeNameStr = nodeName ? nodeName : "Unknown";

        // Get the first property of the node
        FbxProperty prop = node->GetFirstProperty();

        while (prop.IsValid())
        {
            // Get property name
            FbxString propName = prop.GetName();
            std::string propNameStr = propName.Buffer();

            // Check if this is a custom property (user-defined)
            bool isUserProperty = prop.GetFlag(FbxPropertyFlags::eUserDefined);

            // Process numeric properties that match axis-related names
            if (isUserProperty)
            {
                FbxDataType dataType = prop.GetPropertyDataType();

                // Handle different numeric types
                if (dataType.GetType() == eFbxDouble || dataType.GetType() == eFbxFloat)
                {
                    bool shouldScale = false;

                    // Check if property name matches any user-specified property names (case-insensitive)
                    std::wstring propNameWStr = StringToWString(propNameStr);
                    for (const auto& userPropName : g_metadataPropertyNames)
                    {
                        // Case-insensitive comparison
                        if (_wcsicmp(propNameWStr.c_str(), userPropName.c_str()) == 0)
                        {
                            shouldScale = true;
                            break;
                        }
                    }

                    if (shouldScale)
                    {
                        if (dataType.GetType() == eFbxDouble)
                        {
                            FbxDouble oldValue = prop.Get<FbxDouble>();
                            FbxDouble newValue = oldValue * scaleFactor;
                            prop.Set<FbxDouble>(newValue);

                            // Create log message for UI
                            wchar_t logMsg[512];
                            swprintf_s(logMsg, L"    [Metadata] Node '%S': '%S' = %.3f -> %.3f",
                                nodeNameStr.c_str(), propNameStr.c_str(), oldValue, newValue);
                            g_metadataLogs.push_back(logMsg);
                        }
                        else if (dataType.GetType() == eFbxFloat)
                        {
                            FbxFloat oldValue = prop.Get<FbxFloat>();
                            FbxFloat newValue = (FbxFloat)(oldValue * scaleFactor);
                            prop.Set<FbxFloat>(newValue);

                            // Create log message for UI
                            wchar_t logMsg[512];
                            swprintf_s(logMsg, L"    [Metadata] Node '%S': '%S' = %.3f -> %.3f",
                                nodeNameStr.c_str(), propNameStr.c_str(), (double)oldValue, (double)newValue);
                            g_metadataLogs.push_back(logMsg);
                        }
                    }
                }
            }

            // Move to next property
            prop = node->GetNextProperty(prop);
        }
    }

    void RotateNode(FbxNode* node, ResizeAxis axis, double rotationDegrees)
    {
        if (!node || rotationDegrees == 0.0) return;

        // Get the current local rotation
        FbxDouble3 currentRotation = node->LclRotation.Get();

        // Add rotation to the specified axis
        switch (axis)
        {
        case ResizeAxis::X:
            currentRotation[0] += rotationDegrees;
            break;
        case ResizeAxis::Y:
            currentRotation[1] += rotationDegrees;
            break;
        case ResizeAxis::Z:
            currentRotation[2] += rotationDegrees;
            break;
        }

        // Set the new rotation
        node->LclRotation.Set(currentRotation);
    }

    void RotateHierarchy(FbxNode* node, ResizeAxis axis, double rotationDegrees)
    {
        if (!node || rotationDegrees == 0.0) return;

        // Create rotation matrix
        FbxAMatrix rotationMatrix;
        FbxVector4 rotationVector(0, 0, 0, 0);

        switch (axis)
        {
        case ResizeAxis::X:
            rotationVector.Set(rotationDegrees, 0, 0, 0);
            break;
        case ResizeAxis::Y:
            rotationVector.Set(0, rotationDegrees, 0, 0);
            break;
        case ResizeAxis::Z:
            rotationVector.Set(0, 0, rotationDegrees, 0);
            break;
        }

        rotationMatrix.SetR(rotationVector);

        // Apply rotation to all direct children of root (the actual model/skeleton hierarchy)
        for (int i = 0; i < node->GetChildCount(); ++i)
        {
            FbxNode* child = node->GetChild(i);

            // Get current transform
            FbxAMatrix currentTransform = child->EvaluateLocalTransform();

            // Apply rotation
            FbxAMatrix newTransform = rotationMatrix * currentTransform;

            // Extract and set new rotation
            FbxVector4 newRotation = newTransform.GetR();
            child->LclRotation.Set(FbxDouble3(newRotation[0], newRotation[1], newRotation[2]));
        }
    }

    std::wstring GetLastError()
    {
        return g_lastError;
    }
}
