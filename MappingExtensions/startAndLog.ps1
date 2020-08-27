& adb shell am force-stop com.beatgames.beatsaber
& adb shell logcat -c
# & adb shell am start -n com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
Write-Output "Starting logcat to logcat.log..."
& adb logcat > .\logcat.log
