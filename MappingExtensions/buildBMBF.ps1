# Builds a .zip file for loading with BMBF
$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

& $buildScript NDK_PROJECT_PATH=$PSScriptRoot APP_BUILD_SCRIPT=$PSScriptRoot/Android.mk NDK_APPLICATION_MK=$PSScriptRoot/Application.mk
Compress-Archive -Path "./bmbfmod.json","./libs/arm64-v8a/libmappingextensions.so","./libs/arm64-v8a/libbeatsaber-hook_0_4_7.so","./libs/arm64-v8a/libcodegen_0_2_1.so" -DestinationPath "./MappingExtensions_v0.17.1.zip" -Update
