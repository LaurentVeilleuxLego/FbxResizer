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
set(FBX_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/fbx")
set(FBX_SDK_INCLUDE_DIR "${FBX_SDK_ROOT}/include")
set(FBX_SDK_LIB_DIR_DEBUG "${FBX_SDK_ROOT}/lib/x64/debug")
set(FBX_SDK_LIB_DIR_RELEASE "${FBX_SDK_ROOT}/lib/x64/release")

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

# Link Windows libraries and FBX SDK
target_link_libraries(FbxResizer PRIVATE 
    comctl32
    debug "${FBX_SDK_LIB_DIR_DEBUG}/libfbxsdk.lib"
    optimized "${FBX_SDK_LIB_DIR_RELEASE}/libfbxsdk.lib"
    debug "${FBX_SDK_LIB_DIR_DEBUG}/libxml2-md.lib"
    optimized "${FBX_SDK_LIB_DIR_RELEASE}/libxml2-md.lib"
    debug "${FBX_SDK_LIB_DIR_DEBUG}/zlib-md.lib"
    optimized "${FBX_SDK_LIB_DIR_RELEASE}/zlib-md.lib"
)

# Add compiler definitions
target_compile_definitions(FbxResizer PRIVATE 
    FBXSDK_SHARED
)

# Copy FBX SDK DLL to output directory
add_custom_command(TARGET FbxResizer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<$<CONFIG:Debug>:${FBX_SDK_LIB_DIR_DEBUG}/libfbxsdk.dll>$<$<CONFIG:Release>:${FBX_SDK_LIB_DIR_RELEASE}/libfbxsdk.dll>"
    $<TARGET_FILE_DIR:FbxResizer>
    COMMENT "Copying FBX SDK DLL to output directory"
)

# TODO: Add tests and install targets if needed.
