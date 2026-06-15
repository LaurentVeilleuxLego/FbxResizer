#pragma once

#include <string>
#include <vector>

// FBX Processing utilities
// This header defines the interface for FBX file processing operations

namespace FbxProcessor
{
    // Axis enumeration
    enum class ResizeAxis
    {
        X,
        Y,
        Z
    };

    // Result structure
    struct ProcessResult
    {
        bool success;
        std::wstring message;
        std::wstring inputFile;
        std::wstring outputFile;
        std::vector<std::wstring> metadataLogs;  // Log messages for scaled metadata
    };

    // Initialize FBX SDK
    bool InitializeFbxSdk(const std::wstring& sdkPath);

    // Cleanup FBX SDK
    void CleanupFbxSdk();

    // Process a single FBX file
    ProcessResult ProcessFbxFile(
        const std::wstring& inputPath,
        const std::wstring& outputPath,
        ResizeAxis axis,
        double scaleFactor,
        const std::vector<std::wstring>& metadataPropertyNames = {});

    // Get detailed error message
    std::wstring GetLastError();
}
