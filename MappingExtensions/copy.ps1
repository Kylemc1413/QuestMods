Set-StrictMode -Version Latest
$ErrorActionPreference = "Ignore"
$PSDefaultParameterValues['*:ErrorAction']='Ignore'

adb shell am force-stop com.beatgames.beatsaber

# Remove-Item ./obj -r
# Remove-Item ./libs -r

C:\android\sdk\ndk\21.0.6113669\ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk NDK_APPLICATION_MK=./Application.mk V=1
$native_call_success = $?
if (-not $native_call_success)
{
    throw 'error in ndk-build'
}

adb shell am force-stop com.beatgames.beatsaber

gci -rec -file -path libs -filter *.so  |
    % { adb push $_.FullName /sdcard/Android/data/com.beatgames.beatsaber/files/mods/ }

Copy-Item libs/arm*/*.so .
Compress-Archive -LiteralPath libmod2.so, beatonmod.json -DestinationPath menuMod -Force

adb shell am force-stop com.beatgames.beatsaber
adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
