#include <android/log.h>
#include <stdlib.h>
#include <time.h>
#include <stdalign.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <array>
#include <vector>
#include <limits>
#include <map>
#include "../beatsaber-hook/shared/inline-hook/inlineHook.h"
#include "../beatsaber-hook/shared/utils/utils.h"
#include "../beatsaber-hook/shared/libil2cpp/il2cpp-api.h"

//#undef log
//#define log(...) __android_log_print(ANDROID_LOG_INFO, "QuestHook", "[CustomSabers v0.0.1] " __VA_ARGS__)

//Hook offsets
#define set_active_scene_offset 0xD902B4
#define MOD_ID "CustomSabers"
#define VERSION "0.0.1"

using il2cpp_utils::createcsstr;
using il2cpp_utils::GetClassFromName;
static const MethodInfo *(*get_method_from_name)(Il2CppClass *, const char *, int) = nullptr;
static Il2CppObject *(*runtime_invoke)(const MethodInfo *, void *, void **, Il2CppException **) = nullptr;

template <class T>
struct List : Il2CppObject
{
    Array<T> *items;
    int size;
    int version;    // ?
    void *syncRoot; // ?
    uint padding2;  // ?
    uint padding3;  // ?

    //   auto static LIST_ADD_ITEM = reinterpret_cast<function_ptr_t<void, List<T> *, T>>(getRealOffset(0x12105A4));
};

struct UnityObject : Il2CppObject
{
    int *m_cachedPtr;
};
typedef struct
{
    float x;
    float y;
    float z;
    float w;
} Quaternion;

struct SaveDataNoteData : Il2CppObject
{
    float time;
    int lineIndex;
    int lineLayer;
    int type;
    int cutDirection;
};

struct Bounds : Il2CppObject
{
    Vector3 center;
    Vector3 extents;
};

struct Scene : Il2CppObject
{
    int handle;
};

Quaternion ToQuaternion(float pitch, float yaw, float roll) // yaw (Z), pitch (Y), roll (X)
{
    // Abbreviations for the various angular functions
    yaw *= 0.01745329251;
    pitch *= 0.01745329251;
    roll *= 0.01745329251;
    float rollOver2 = roll * 0.5f;
    float sinRollOver2 = sinf(rollOver2);
    float cosRollOver2 = cosf(rollOver2);
    float pitchOver2 = pitch * 0.5f;
    float sinPitchOver2 = sinf(pitchOver2);
    float cosPitchOver2 = cosf(pitchOver2);
    float yawOver2 = yaw * 0.5f;
    float sinYawOver2 = sinf(yawOver2);
    float cosYawOver2 = cosf(yawOver2);
    Quaternion result;
    result.w = cosYawOver2 * cosPitchOver2 * cosRollOver2 + sinYawOver2 * sinPitchOver2 * sinRollOver2;
    result.x = cosYawOver2 * sinPitchOver2 * cosRollOver2 + sinYawOver2 * cosPitchOver2 * sinRollOver2;
    result.y = sinYawOver2 * cosPitchOver2 * cosRollOver2 - cosYawOver2 * sinPitchOver2 * sinRollOver2;
    result.z = cosYawOver2 * cosPitchOver2 * sinRollOver2 - sinYawOver2 * sinPitchOver2 * cosRollOver2;

    return result;
}

Vector3 AddVectors(Vector3 left, Vector3 right)
{
    Vector3 result;
    result.x = left.x + right.x;
    result.y = left.y + right.y;
    result.z = left.z + right.z;
    return result;
}
Vector3 SubtractVectors(Vector3 left, Vector3 right)
{
    Vector3 result;
    result.x = left.x - right.x;
    result.y = left.y - right.y;
    result.z = left.z - right.z;
    return result;
}
float VectorMagnitude(Vector3 vector)
{
    return sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}
static Il2CppClass *sceneClass;
static Il2CppClass *assetBundleClass;
static Il2CppClass *assetBundleCreateRequestClass;
static const MethodInfo *sceneNameMethodInfo;
static const MethodInfo *assetBundleFromFileAsync;
static const MethodInfo *assetBundleFromAsync;
static const MethodInfo *loadAssetAsync;
MAKE_HOOK(set_active_scene, set_active_scene_offset, bool, int scene)
{
    log("Called set_active_scene hook");
    bool result = set_active_scene(scene);

    if (sceneClass == nullptr)
        sceneClass = GetClassFromName("UnityEngine.SceneManagement", "Scene");
    //Get scene name method
    if (sceneNameMethodInfo == nullptr)
    {
        sceneNameMethodInfo = get_method_from_name(sceneClass, "GetNameInternal", 1);
    }
    //Get Scene Name

    Il2CppException *exception = nullptr;
    void *sceneNameparams[] = {&scene};
    auto nameResult = runtime_invoke(sceneNameMethodInfo, nullptr, sceneNameparams, &exception);
    cs_string *csName = reinterpret_cast<cs_string *>(nameResult);
    auto sceneName = to_utf8(csstrtostr(csName)).c_str();

    //Code to run if menuCore
    if (std::strncmp(sceneName, "MenuCore", 8) == 0)
    {
        log("MenuCore Scene");
        if (assetBundleClass == nullptr)
            assetBundleClass = GetClassFromName("UnityEngine", "AssetBundle");
        if (assetBundleCreateRequestClass == nullptr)
            assetBundleCreateRequestClass = GetClassFromName("UnityEngine", "AssetBundleCreateRequest");

        if (assetBundleFromFileAsync == nullptr)
            assetBundleFromFileAsync = get_method_from_name(assetBundleClass, "LoadFromFileAsync", 1);

        if (assetBundleFromAsync == nullptr)
            assetBundleFromAsync = get_method_from_name(assetBundleCreateRequestClass, "get_AssetBundle", 0);

        if (loadAssetAsync == nullptr)
            loadAssetAsync = get_method_from_name(assetBundleClass, "LoadAssetAsync", 3);

        //Attempt to load assetbundle
        //void* static LoadFromFileAsync(fileName): 0x1278B44
        //void* get_AssetBundle(asyncBundle): 0x1278D60
        //void* LoadAssetAsync(bundle, type, name_from_bundle): 0x5CD740
        cs_string *assetFilePath = createcsstr("/sdcard/Android/data/com.beatgames.beatsaber/files/sabers/tesla3.saber");
        void *fromFileParams[] = {assetFilePath};
        void *asyncBundle = runtime_invoke(assetBundleFromFileAsync, nullptr, fromFileParams, &exception);
                log("Grabbed Async bundle");
   
   //     void *assetBundle = runtime_invoke(assetBundleFromAsync, asyncBundle, nullptr, &exception);
   //     log("Grabbed Asset bundle");
      //  void *assetAsync = runtime_invoke(loadAssetAsync, assetBundle, )
    }
    log("End set_active_scene hook");
    return result;
}

__attribute__((constructor)) void lib_main()
{

    log("Installing Custom Sabers Hooks!");
    INSTALL_HOOK(set_active_scene);
    log("Installed Custom Sabers Hooks!");
    log("Getting il2cpp api functions for Custom Sabers.");
    if (get_method_from_name == nullptr || runtime_invoke == nullptr)
    {
        void *imagehandle = dlopen("/data/app/com.beatgames.beatsaber-1/lib/arm/libil2cpp.so", 1);
        if (get_method_from_name == nullptr)

            *(void **)(&get_method_from_name) = dlsym(imagehandle, "il2cpp_class_get_method_from_name");
        if (runtime_invoke == nullptr)
            *(void **)(&runtime_invoke) = dlsym(imagehandle, "il2cpp_runtime_invoke");
        dlclose(imagehandle);
    }
    log("Got il2cpp api functions for Custom Sabers.");
}
