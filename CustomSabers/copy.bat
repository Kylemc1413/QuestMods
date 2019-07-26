call adb shell am force-stop com.beatgames.beatsaber

call D:/Android/android-ndk-r20/ndk-build.cmd NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk NDK_APPLICATION_MK=./Application.mk

call adb push libs/armeabi-v7a/libcustomsabers.so /sdcard/Android/data/com.beatgames.beatsaber/files/mods/libcustomsabers.so
call adb push tesla3.saber /sdcard/Android/data/com.beatgames.beatsaber/files/sabers/tesla3.saber
call adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity

