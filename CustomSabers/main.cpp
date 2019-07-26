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

#undef log
#define log(...) __android_log_print(ANDROID_LOG_INFO, "QuestHook", "[CustomSabers v0.0.1] " __VA_ARGS__)

//Hook offsets
#define set_active_scene_offset 0xD902B4

using il2cpp_utils::GetClassFromName;
void dumpBytes(int before, int after, void* loc) {
    for (int i = -before; i < after; i++) {
        int val = *(int*)((int)loc + i);
        log("4 Bytes at: %p has hex value: %08x", (void*)((int)loc + i), val);
    }
}

template <class T>
struct List : Object
{
    Array<T> *items;
    int size;
    int version;    // ?
    void *syncRoot; // ?
    uint padding2;  // ?
    uint padding3;  // ?

    //   auto static LIST_ADD_ITEM = reinterpret_cast<function_ptr_t<void, List<T> *, T>>(getRealOffset(0x12105A4));
};

struct UnityObject : Object
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

struct SaveDataNoteData : Object
{
    float time;
    int lineIndex;
    int lineLayer;
    int type;
    int cutDirection;
};

struct Bounds : Object
{
    Vector3 center;
    Vector3 extents;
};

struct Scene : Object
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
static const MethodInfo *sceneNameMethodInfo;
static const MethodInfo *(*get_methods)(Il2CppClass *, void **) = nullptr;
static Il2CppObject *(*runtime_invoke)(const MethodInfo *, void *, void **, Il2CppException **) = nullptr;

MAKE_HOOK(set_active_scene, set_active_scene_offset, bool, int scene)
{
    log("Called set_active_scene hook");

 //   bool result = set_active_scene(scene);
 //   log("Result %s", result ? "true" : "false");
 
    if (sceneClass == nullptr)
        sceneClass = GetClassFromName("UnityEngine.SceneManagement", "Scene");
    if (get_methods == nullptr || runtime_invoke == nullptr)
    {
        void *imagehandle = dlopen("/data/app/com.beatgames.beatsaber-1/lib/arm/libil2cpp.so", 1);
        if (get_methods == nullptr)
            *(void **)(&get_methods) = dlsym(imagehandle, "il2cpp_class_get_methods");
        if (runtime_invoke == nullptr)
            *(void **)(&runtime_invoke) = dlsym(imagehandle, "il2cpp_runtime_invoke");
        dlclose(imagehandle);
    }
  void *iter = nullptr;
    MethodInfo const *m;
    if (sceneNameMethodInfo == nullptr)
        while ((m = get_methods(sceneClass, &iter)) != nullptr)
        {
            if (std::strncmp(m->name, "GetNameInternal", 16) == 0)
            {
                //          log("Stretchable Obstacle Method: %s", m->name);
                sceneNameMethodInfo = m;
                //       log("SetSizeMethod: %i", SetSizeMethodInfo->parameters_count);
                break;
            }
        }
    Il2CppException *exception = nullptr;
  //  log("Scene Name Method params: %i", sceneNameMethodInfo->parameters_count);
  //      log("Scene Name Method name: %s", sceneNameMethodInfo->name);
        

    void*  params[] = {&scene};
  //  public string get_name();
    auto result2 = runtime_invoke(sceneNameMethodInfo, nullptr, params, &exception);
    cs_string * csName = reinterpret_cast<cs_string*>(result2);
    char sceneName[100];
    csstrtostr(csName, sceneName);
    log("Scene Name: %s", sceneName);
  //  log("Scene Name: %s", reinterpret_cast<const char*>(result));

        log("End set_active_scene hook");
    return set_active_scene(scene);
}

__attribute__((constructor)) void lib_main()
{

    log("Installing Custom Sabers Hooks!");
    INSTALL_HOOK(set_active_scene);
    log("Installed Custom Sabers Hooks!");
}
