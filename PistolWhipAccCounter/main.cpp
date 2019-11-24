#include <android/log.h>
#include <stdlib.h>
#include <time.h>
#include <stdalign.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <array>
#include <vector>
#include <limits>
#include <map>
#include "../beatsaber-hook/shared/utils/logging.h"
#include "../beatsaber-hook/shared/inline-hook/inlineHook.h"
#include "../beatsaber-hook/shared/utils/utils.h"
#include "../beatsaber-hook/shared/utils/utils-functions.h"
#include "../beatsaber-hook/shared/utils/typedefs.h"
//#define line_y_pos_for_line_layer_offset 0x4F5AC0

#define MOD_ID "AccCounter"
#define VERSION "0.0.1"

#define GunAmmoDisplayUpdateOffset 0x15E2798
#define PlayerActionManagerGameStartOffset 0x1724B54

using il2cpp_utils::GetClassFromName;
using namespace il2cpp_functions;
using TYPEDEFS_H::Quaternion;

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
static Il2CppObject* GameData;
MAKE_HOOK(PlayerActionManagerGameStart, PlayerActionManagerGameStartOffset, void, void* self, void* e)
{
    log_print(INFO, "Called PlayerActionManager GameStart Hook!");
    PlayerActionManagerGameStart(self, e);
    GameData = nullptr;
    GameData = field_get_value_object(class_get_field_from_name(GetClassFromName("", "PlayerActionManager"), "playerData"),
                                                                                    reinterpret_cast<Il2CppObject*>(self));
    log_print(INFO, "Finished PlayerActionManager GameStart Hook!");
}
MAKE_HOOK(GunAmmoDisplayUpdate, GunAmmoDisplayUpdateOffset, void, void* self)
{
    return;
    GunAmmoDisplayUpdate(self);
    if(GameData == nullptr) return;
    Il2CppObject* displayTextObj = field_get_value_object(class_get_field_from_name(GetClassFromName("", "GunAmmoDisplay"), "displayText"),
    reinterpret_cast<Il2CppObject*>(self));
    static FieldInfo* textField = class_get_field_from_name(GetClassFromName("TMPro", "TextMeshPro"), "m_text");
    Il2CppString* displayText =  reinterpret_cast<Il2CppString *>(field_get_value_object(textField, displayTextObj));
    std::string text = to_utf8(csstrtostr(displayText));
    float accuracy = *(reinterpret_cast<float *>(object_unbox(
        field_get_value_object(class_get_field_from_name(GetClassFromName("", "GameData"), "accuracy"), GameData))));
    text += " (" + std::to_string(accuracy) +")";
    field_set_value(displayTextObj, textField, &text);
}

__attribute__((constructor)) void lib_main()
{

    log_print(INFO, "Installing AccCounter Hooks!");
    INSTALL_HOOK(PlayerActionManagerGameStart);
    INSTALL_HOOK(GunAmmoDisplayUpdate);
    log_print(INFO, "Installed AccCounter Hooks!");
    log_print(INFO, "Initializing Il2Cpp Functions for AccCounter");
    il2cpp_functions::Init();
    log_print(INFO, "Initialized Il2Cpp Functions for AccCounter");
}
static Il2CppClass *resourcesClass;
static const MethodInfo *resourcesFindObjectsOfTypeAll;
void *GetFirstObjectOfType(Il2CppClass *klass)
{
    if(resourcesClass == nullptr)
    resourcesClass = GetClassFromName("UnityEngine", "Resources");
    if(resourcesFindObjectsOfTypeAll == nullptr)
    resourcesFindObjectsOfTypeAll = class_get_method_from_name(resourcesClass, "FindObjectsOfTypeAll", 1);
    Il2CppException *exception;
    void *params[] = {type_get_object(class_get_type(klass))};
    Array<void *> *objects = reinterpret_cast<Array<void *> *>(runtime_invoke(resourcesFindObjectsOfTypeAll, nullptr, params, &exception));
    if (objects != nullptr)
    {
        return objects->values[0];
    }
    else
    {
        return nullptr;
    }
}