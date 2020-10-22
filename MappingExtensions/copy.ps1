Set-StrictMode -Version Latest
$ErrorActionPreference = "Ignore"
$PSDefaultParameterValues['*:ErrorAction']='Ignore'

adb shell am force-stop com.beatgames.beatsaber

# Remove-Item ./obj -r
# Remove-Item ./libs -r

& $PSScriptRoot/build.ps1
$native_call_success = $?
if (-not $native_call_success)
{
    throw 'error in ndk-build'
}

adb shell am force-stop com.beatgames.beatsaber

gci -rec -file -path libs -filter *extensions.so  |
    % { adb push $_.FullName /sdcard/Android/data/com.beatgames.beatsaber/files/mods/ }
gci -rec -file -path libs -filter *codegen*.so  |
    % { adb push $_.FullName /sdcard/Android/data/com.beatgames.beatsaber/files/libs/ }
gci -rec -file -path libs -filter *beatsaber-hook*.so  |
    % { adb push $_.FullName /sdcard/Android/data/com.beatgames.beatsaber/files/libs/ }

adb shell am force-stop com.beatgames.beatsaber
