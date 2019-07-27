call adb shell am force-stop com.beatgames.beatsaber

call D:/Android/android-ndk-r20/ndk-build.cmd NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk NDK_APPLICATION_MK=./Application.mk

call adb push libs/armeabi-v7a/libcustomsabers.so /sdcard/Android/data/com.beatgames.beatsaber/files/mods/libcustomsabers.so
call adb push testSaber.qsaber /sdcard/Android/data/com.beatgames.beatsaber/files/sabers/testSaber.qsaber
call adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity

